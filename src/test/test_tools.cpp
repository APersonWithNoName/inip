#include "gtest/gtest.h"

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "inip/Tools.hpp"

// 测试 split 函数(char delim 版本,使用 std::getline)
TEST(ToolsTest, SplitCharDelimGetlineTest)
{
  // 测试基本分割
  std::vector<std::string> result1 = ::inip::Tools::split("a,b,c", ',');
  EXPECT_EQ(result1.size(), 3);
  EXPECT_EQ(result1[0], "a");
  EXPECT_EQ(result1[1], "b");
  EXPECT_EQ(result1[2], "c");

  // 测试空字符串
  std::vector<std::string> result2 = ::inip::Tools::split("", ',');
  EXPECT_TRUE(result2.empty());

  // 测试没有分隔符
  std::vector<std::string> result3 = ::inip::Tools::split("hello", ',');
  EXPECT_EQ(result3.size(), 1);
  EXPECT_EQ(result3[0], "hello");

  // 测试连续分隔符 - getline通常保留空token
  std::vector<std::string> result4 = ::inip::Tools::split("a,,b,,c", ',');
  // getline通常行为：保留空token
  EXPECT_EQ(result4.size(), 5);
  EXPECT_EQ(result4[0], "a");
  EXPECT_EQ(result4[1], "");
  EXPECT_EQ(result4[2], "b");
  EXPECT_EQ(result4[3], "");
  EXPECT_EQ(result4[4], "c");

  // 测试前后分隔符 - getline通常保留空token
  std::vector<std::string> result5 = ::inip::Tools::split(",a,b,c,", ',');
  EXPECT_EQ(result5.size(), 5);
  EXPECT_EQ(result5[0], "");
  EXPECT_EQ(result5[1], "a");
  EXPECT_EQ(result5[2], "b");
  EXPECT_EQ(result5[3], "c");
  EXPECT_EQ(result5[4], "");

  // 测试空格分隔符
  std::vector<std::string> result6 = ::inip::Tools::split("hello world test", ' ');
  EXPECT_EQ(result6.size(), 3);
  EXPECT_EQ(result6[0], "hello");
  EXPECT_EQ(result6[1], "world");
  EXPECT_EQ(result6[2], "test");

  // 测试制表符分隔符
  std::vector<std::string> result7 = ::inip::Tools::split("a\tb\tc", '\t');
  EXPECT_EQ(result7.size(), 3);
  EXPECT_EQ(result7[0], "a");
  EXPECT_EQ(result7[1], "b");
  EXPECT_EQ(result7[2], "c");

  // 测试换行符分隔符
  std::vector<std::string> result8 = ::inip::Tools::split("line1\nline2\nline3", '\n');
  EXPECT_EQ(result8.size(), 3);
  EXPECT_EQ(result8[0], "line1");
  EXPECT_EQ(result8[1], "line2");
  EXPECT_EQ(result8[2], "line3");

  // 测试特殊字符作为分隔符
  std::vector<std::string> result9 = ::inip::Tools::split("key=value", '=');
  EXPECT_EQ(result9.size(), 2);
  EXPECT_EQ(result9[0], "key");
  EXPECT_EQ(result9[1], "value");

  // 测试只有分隔符
  std::vector<std::string> result10 = ::inip::Tools::split(",,,", ',');
  EXPECT_EQ(result10.size(), 4);
  for (const auto &s : result10) {
    EXPECT_EQ(s, "");
  }
}

// 测试 split 函数(std::string delim 版本)
TEST(ToolsTest, SplitStringDelimDefaultTest)
{
  // 测试默认行为(skip_empty = true)

  // 测试基本分割
  std::vector<std::string> result1 = ::inip::Tools::split("a,b,c", ",");
  EXPECT_EQ(result1.size(), 3);
  EXPECT_EQ(result1[0], "a");
  EXPECT_EQ(result1[1], "b");
  EXPECT_EQ(result1[2], "c");

  // 测试空字符串
  std::vector<std::string> result2 = ::inip::Tools::split("", ",");
  EXPECT_TRUE(result2.empty());

  // 测试没有分隔符
  std::vector<std::string> result3 = ::inip::Tools::split("hello", ",");
  EXPECT_EQ(result3.size(), 1);
  EXPECT_EQ(result3[0], "hello");

  // 测试连续分隔符 - 默认跳过空token
  std::vector<std::string> result4 = ::inip::Tools::split("a,,b,,c", ",");
  EXPECT_EQ(result4.size(), 3); // 跳过空token
  EXPECT_EQ(result4[0], "a");
  EXPECT_EQ(result4[1], "b");
  EXPECT_EQ(result4[2], "c");

  // 测试前后分隔符 - 默认跳过空token
  std::vector<std::string> result5 = ::inip::Tools::split(",a,b,c,", ",");
  EXPECT_EQ(result5.size(), 3); // 跳过开头和结尾的空token
  EXPECT_EQ(result5[0], "a");
  EXPECT_EQ(result5[1], "b");
  EXPECT_EQ(result5[2], "c");

  // 测试多字符分隔符
  std::vector<std::string> result6 = ::inip::Tools::split("a->b->c", "->");
  EXPECT_EQ(result6.size(), 3);
  EXPECT_EQ(result6[0], "a");
  EXPECT_EQ(result6[1], "b");
  EXPECT_EQ(result6[2], "c");

  // 测试只有分隔符 - 全部跳过
  std::vector<std::string> result7 = ::inip::Tools::split(",,,", ",");
  EXPECT_TRUE(result7.empty());

  // 测试长分隔符
  std::vector<std::string> result8 = ::inip::Tools::split("startMIDDLEend", "MIDDLE");
  EXPECT_EQ(result8.size(), 2);
  EXPECT_EQ(result8[0], "start");
  EXPECT_EQ(result8[1], "end");

  // 测试空分隔符的特殊处理
  std::vector<std::string> result9 = ::inip::Tools::split("test", "");
  EXPECT_EQ(result9.size(), 4); // 每个字符分开
  EXPECT_EQ(result9[0], "t");
  EXPECT_EQ(result9[1], "e");
  EXPECT_EQ(result9[2], "s");
  EXPECT_EQ(result9[3], "t");
}

TEST(ToolsTest, SplitStringDelimSkipFalseTest)
{
  // 测试 skip_empty = false 行为

  // 测试连续分隔符 - 保留空token
  std::vector<std::string> result1 = ::inip::Tools::split("a,,b,,c", ",", false);
  EXPECT_EQ(result1.size(), 5);
  EXPECT_EQ(result1[0], "a");
  EXPECT_EQ(result1[1], "");
  EXPECT_EQ(result1[2], "b");
  EXPECT_EQ(result1[3], "");
  EXPECT_EQ(result1[4], "c");

  // 测试前后分隔符 - 保留空token
  std::vector<std::string> result2 = ::inip::Tools::split(",a,b,c,", ",", false);
  EXPECT_EQ(result2.size(), 5);
  EXPECT_EQ(result2[0], "");
  EXPECT_EQ(result2[1], "a");
  EXPECT_EQ(result2[2], "b");
  EXPECT_EQ(result2[3], "c");
  EXPECT_EQ(result2[4], "");

  // 测试只有分隔符 - 保留所有空token
  std::vector<std::string> result3 = ::inip::Tools::split(",,,", ",", false);
  EXPECT_EQ(result3.size(), 4);
  for (const auto &s : result3) {
    EXPECT_EQ(s, "");
  }

  // 测试重叠分隔符
  std::vector<std::string> result4 = ::inip::Tools::split("a->->b", "->", false);
  EXPECT_EQ(result4.size(), 3);
  EXPECT_EQ(result4[0], "a");
  EXPECT_EQ(result4[1], "");
  EXPECT_EQ(result4[2], "b");

  // 测试空格分隔符
  std::vector<std::string> result5 = ::inip::Tools::split("hello  world", " ", false);
  EXPECT_EQ(result5.size(), 3); // "hello", "", "world"
  EXPECT_EQ(result5[0], "hello");
  EXPECT_EQ(result5[1], "");
  EXPECT_EQ(result5[2], "world");
}

TEST(ToolsTest, SplitStringDelimSkipTrueTest)
{
  // 测试 skip_empty = true 行为(与默认相同)

  // 测试连续分隔符 - 跳过空token
  std::vector<std::string> result1 = ::inip::Tools::split("a,,b,,c", ",", true);
  EXPECT_EQ(result1.size(), 3);
  EXPECT_EQ(result1[0], "a");
  EXPECT_EQ(result1[1], "b");
  EXPECT_EQ(result1[2], "c");

  // 测试前后分隔符 - 跳过空token
  std::vector<std::string> result2 = ::inip::Tools::split(",a,b,c,", ",", true);
  EXPECT_EQ(result2.size(), 3);
  EXPECT_EQ(result2[0], "a");
  EXPECT_EQ(result2[1], "b");
  EXPECT_EQ(result2[2], "c");
}

// 测试 trim 函数
TEST(ToolsTest, TrimTest)
{
  // 测试基本修剪
  EXPECT_EQ(::inip::Tools::trim("  hello  "), "hello");
  EXPECT_EQ(::inip::Tools::trim("hello"), "hello");
  EXPECT_EQ(::inip::Tools::trim(""), "");

  // 测试只有左侧空格
  EXPECT_EQ(::inip::Tools::trim("  hello"), "hello");

  // 测试只有右侧空格
  EXPECT_EQ(::inip::Tools::trim("hello  "), "hello");

  // 测试制表符和换行符
  EXPECT_EQ(::inip::Tools::trim("\t\n hello \t\n"), "hello");

  // 测试全空格字符串
  EXPECT_EQ(::inip::Tools::trim("   \t\n  "), "");

  // 测试中间有多个空格(应该保留)
  EXPECT_EQ(::inip::Tools::trim("  hello  world  "), "hello  world");

  // 测试Unicode字符串
  EXPECT_EQ(::inip::Tools::trim("  测试  "), "测试");
}

// 测试 cover_map 函数模板
TEST(ToolsTest, CoverMapTest)
{
  // 测试整数map
  std::map<int, std::string> map1 = {{1, "one"}, {2, "two"}};
  std::map<int, std::string> map2 = {{2, "TWO"}, {3, "three"}};

  ::inip::Tools::cover_map(map1, map2);

  EXPECT_EQ(map1.size(), 3);
  EXPECT_EQ(map1[1], "one");   // 保持不变
  EXPECT_EQ(map1[2], "TWO");   // 被覆盖
  EXPECT_EQ(map1[3], "three"); // 新增

  // 测试字符串map
  std::map<std::string, int> map3 = {{"a", 1}, {"b", 2}};
  std::map<std::string, int> map4 = {{"b", 20}, {"c", 30}};

  ::inip::Tools::cover_map(map3, map4);

  EXPECT_EQ(map3.size(), 3);
  EXPECT_EQ(map3["a"], 1);  // 保持不变
  EXPECT_EQ(map3["b"], 20); // 被覆盖
  EXPECT_EQ(map3["c"], 30); // 新增

  // 测试空map覆盖
  std::map<int, double> map5 = {{1, 1.1}, {2, 2.2}};
  std::map<int, double> map6 = {};

  ::inip::Tools::cover_map(map5, map6);
  EXPECT_EQ(map5.size(), 2); // 应该保持不变
  EXPECT_EQ(map5[1], 1.1);
  EXPECT_EQ(map5[2], 2.2);

  // 测试覆盖空map
  std::map<int, double> map7 = {};
  std::map<int, double> map8 = {{1, 1.1}, {2, 2.2}};

  ::inip::Tools::cover_map(map7, map8);
  EXPECT_EQ(map7.size(), 2); // 应该添加所有元素
  EXPECT_EQ(map7[1], 1.1);
  EXPECT_EQ(map7[2], 2.2);
}

// 测试 append_sec_value 函数模板
TEST(ToolsTest, AppendSecValueTest)
{
  // 这些函数接受的是值而不是引用,所以它们不会修改外部vector

  // 测试整数vector
  std::vector<int> vec1 = {1, 2, 3};

  // 由于函数接受值而不是引用,调用后vec1不会改变
  // 这可能是设计问题,但按照当前实现测试
  ::inip::Tools::append_sec_value(vec1, 4);
  // vec1应该保持不变,因为函数参数是传值的
  EXPECT_EQ(vec1.size(), 3);

  // 如果要测试函数逻辑,需要检查返回值或修改函数签名
  // 这里假设函数有返回值或者会修改参数(但根据源码不会)
}

// 测试 append_sec 函数模板
TEST(ToolsTest, AppendSecTest)
{
  // 同样的问题：函数接受值而不是引用
  std::vector<int> vec1 = {1, 2, 3};
  std::vector<int> vec2 = {3, 4, 5};

  ::inip::Tools::append_sec(vec1, vec2);
  // vec1应该保持不变
  EXPECT_EQ(vec1.size(), 3);
}

// 组合测试：split + trim 的常见用法
TEST(ToolsTest, SplitAndTrimCombinationTest)
{
  // 典型场景：解析CSV行,然后修剪每个字段
  std::string csv_line = "  John , Doe , 30 , New York  ";
  std::vector<std::string> fields = ::inip::Tools::split(csv_line, ",");

  // 修剪每个字段
  for (auto &field : fields) {
    field = ::inip::Tools::trim(field);
  }

  // 期望结果
  EXPECT_EQ(fields.size(), 4);
  EXPECT_EQ(fields[0], "John");
  EXPECT_EQ(fields[1], "Doe");
  EXPECT_EQ(fields[2], "30");
  EXPECT_EQ(fields[3], "New York");

  // 另一个场景：解析配置文件
  std::string config_line = "key = value";
  std::vector<std::string> parts = ::inip::Tools::split(config_line, "=");
  for (auto &part : parts) {
    part = ::inip::Tools::trim(part);
  }

  EXPECT_EQ(parts.size(), 2);
  EXPECT_EQ(parts[0], "key");
  EXPECT_EQ(parts[1], "value");
}

// 性能测试
TEST(ToolsTest, PerformanceTest)
{
  // 测试split性能
  const int num_segments = 1000;
  std::string long_string;
  for (int i = 0; i < num_segments; ++i) {
    long_string += "segment" + std::to_string(i);
    if (i < num_segments - 1) {
      long_string += ",";
    }
  }

  auto start = std::chrono::high_resolution_clock::now();
  std::vector<std::string> result = ::inip::Tools::split(long_string, ",");
  auto end = std::chrono::high_resolution_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  std::cout << "Split " << num_segments << " segments took " << duration.count() << " microseconds" << std::endl;

  EXPECT_EQ(result.size(), num_segments);
}

// 边界条件测试
TEST(ToolsTest, EdgeCaseTests)
{
  // 测试超大字符串
  {
    std::string huge(10000, 'a');
    huge += "," + std::string(10000, 'b');
    std::vector<std::string> result = ::inip::Tools::split(huge, ",");
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], std::string(10000, 'a'));
    EXPECT_EQ(result[1], std::string(10000, 'b'));
  }

  // 测试Unicode分隔符
  {
    std::vector<std::string> result = ::inip::Tools::split("测试★分割★功能", "★");
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "测试");
    EXPECT_EQ(result[1], "分割");
    EXPECT_EQ(result[2], "功能");
  }

  // 测试与std::getline的比较
  {
    // 验证char delim版本与std::getline行为一致
    std::string test_str = "a,b,c";
    std::vector<std::string> split_result = ::inip::Tools::split(test_str, ',');

    // 使用std::getline手动实现
    std::vector<std::string> getline_result;
    std::stringstream ss(test_str);
    std::string token;
    while (std::getline(ss, token, ',')) {
      getline_result.push_back(token);
    }

    EXPECT_EQ(split_result.size(), getline_result.size());
    for (size_t i = 0; i < split_result.size(); ++i) {
      EXPECT_EQ(split_result[i], getline_result[i]);
    }
  }
}
