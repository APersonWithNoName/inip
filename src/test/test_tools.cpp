#include "gtest/gtest.h"

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "inip/Tools.hpp"

TEST(ToolsTest, SplitCharDelimGetlineTest)
{
  std::vector<std::string> result1 = ::inip::Tools::split("a,b,c", ',');
  EXPECT_EQ(result1.size(), 3);
  EXPECT_EQ(result1[0], "a");
  EXPECT_EQ(result1[1], "b");
  EXPECT_EQ(result1[2], "c");

  std::vector<std::string> result2 = ::inip::Tools::split("", ',');
  EXPECT_TRUE(result2.empty());

  std::vector<std::string> result3 = ::inip::Tools::split("hello", ',');
  EXPECT_EQ(result3.size(), 1);
  EXPECT_EQ(result3[0], "hello");

  std::vector<std::string> result4 = ::inip::Tools::split("a,,b,,c", ',');
  EXPECT_EQ(result4.size(), 5);
  EXPECT_EQ(result4[0], "a");
  EXPECT_EQ(result4[1], "");
  EXPECT_EQ(result4[2], "b");
  EXPECT_EQ(result4[3], "");
  EXPECT_EQ(result4[4], "c");

  std::vector<std::string> result5 = ::inip::Tools::split(",a,b,c,", ',');
  EXPECT_EQ(result5.size(), 5);
  EXPECT_EQ(result5[0], "");
  EXPECT_EQ(result5[1], "a");
  EXPECT_EQ(result5[2], "b");
  EXPECT_EQ(result5[3], "c");
  EXPECT_EQ(result5[4], "");

  std::vector<std::string> result6 = ::inip::Tools::split("hello world test", ' ');
  EXPECT_EQ(result6.size(), 3);
  EXPECT_EQ(result6[0], "hello");
  EXPECT_EQ(result6[1], "world");
  EXPECT_EQ(result6[2], "test");

  std::vector<std::string> result7 = ::inip::Tools::split("a\tb\tc", '\t');
  EXPECT_EQ(result7.size(), 3);
  EXPECT_EQ(result7[0], "a");
  EXPECT_EQ(result7[1], "b");
  EXPECT_EQ(result7[2], "c");

  std::vector<std::string> result8 = ::inip::Tools::split("line1\nline2\nline3", '\n');
  EXPECT_EQ(result8.size(), 3);
  EXPECT_EQ(result8[0], "line1");
  EXPECT_EQ(result8[1], "line2");
  EXPECT_EQ(result8[2], "line3");

  std::vector<std::string> result9 = ::inip::Tools::split("key=value", '=');
  EXPECT_EQ(result9.size(), 2);
  EXPECT_EQ(result9[0], "key");
  EXPECT_EQ(result9[1], "value");

  std::vector<std::string> result10 = ::inip::Tools::split(",,,", ',');
  EXPECT_EQ(result10.size(), 4);
  for (const auto &s : result10) {
    EXPECT_EQ(s, "");
  }
}

TEST(ToolsTest, SplitStringDelimDefaultTest)
{
  std::vector<std::string> result1 = ::inip::Tools::split("a,b,c", ",");
  EXPECT_EQ(result1.size(), 3);
  EXPECT_EQ(result1[0], "a");
  EXPECT_EQ(result1[1], "b");
  EXPECT_EQ(result1[2], "c");

  std::vector<std::string> result2 = ::inip::Tools::split("", ",");
  EXPECT_TRUE(result2.empty());

  std::vector<std::string> result3 = ::inip::Tools::split("hello", ",");
  EXPECT_EQ(result3.size(), 1);
  EXPECT_EQ(result3[0], "hello");

  std::vector<std::string> result4 = ::inip::Tools::split("a,,b,,c", ",");
  EXPECT_EQ(result4.size(), 3);
  EXPECT_EQ(result4[0], "a");
  EXPECT_EQ(result4[1], "b");
  EXPECT_EQ(result4[2], "c");

  std::vector<std::string> result5 = ::inip::Tools::split(",a,b,c,", ",");
  EXPECT_EQ(result5.size(), 3);
  EXPECT_EQ(result5[0], "a");
  EXPECT_EQ(result5[1], "b");
  EXPECT_EQ(result5[2], "c");

  std::vector<std::string> result6 = ::inip::Tools::split("a->b->c", "->");
  EXPECT_EQ(result6.size(), 3);
  EXPECT_EQ(result6[0], "a");
  EXPECT_EQ(result6[1], "b");
  EXPECT_EQ(result6[2], "c");

  std::vector<std::string> result7 = ::inip::Tools::split(",,,", ",");
  EXPECT_TRUE(result7.empty());

  std::vector<std::string> result8 = ::inip::Tools::split("startMIDDLEend", "MIDDLE");
  EXPECT_EQ(result8.size(), 2);
  EXPECT_EQ(result8[0], "start");
  EXPECT_EQ(result8[1], "end");

  std::vector<std::string> result9 = ::inip::Tools::split("test", "");
  EXPECT_EQ(result9.size(), 4);
  EXPECT_EQ(result9[0], "t");
  EXPECT_EQ(result9[1], "e");
  EXPECT_EQ(result9[2], "s");
  EXPECT_EQ(result9[3], "t");
}

TEST(ToolsTest, SplitStringDelimSkipFalseTest)
{
  std::vector<std::string> result1 = ::inip::Tools::split("a,,b,,c", ",", false);
  EXPECT_EQ(result1.size(), 5);
  EXPECT_EQ(result1[0], "a");
  EXPECT_EQ(result1[1], "");
  EXPECT_EQ(result1[2], "b");
  EXPECT_EQ(result1[3], "");
  EXPECT_EQ(result1[4], "c");

  std::vector<std::string> result2 = ::inip::Tools::split(",a,b,c,", ",", false);
  EXPECT_EQ(result2.size(), 5);
  EXPECT_EQ(result2[0], "");
  EXPECT_EQ(result2[1], "a");
  EXPECT_EQ(result2[2], "b");
  EXPECT_EQ(result2[3], "c");
  EXPECT_EQ(result2[4], "");

  std::vector<std::string> result3 = ::inip::Tools::split(",,,", ",", false);
  EXPECT_EQ(result3.size(), 4);
  for (const auto &s : result3) {
    EXPECT_EQ(s, "");
  }

  std::vector<std::string> result4 = ::inip::Tools::split("a->->b", "->", false);
  EXPECT_EQ(result4.size(), 3);
  EXPECT_EQ(result4[0], "a");
  EXPECT_EQ(result4[1], "");
  EXPECT_EQ(result4[2], "b");

  std::vector<std::string> result5 = ::inip::Tools::split("hello  world", " ", false);
  EXPECT_EQ(result5.size(), 3); // "hello", "", "world"
  EXPECT_EQ(result5[0], "hello");
  EXPECT_EQ(result5[1], "");
  EXPECT_EQ(result5[2], "world");
}

TEST(ToolsTest, SplitStringDelimSkipTrueTest)
{
  std::vector<std::string> result1 = ::inip::Tools::split("a,,b,,c", ",", true);
  EXPECT_EQ(result1.size(), 3);
  EXPECT_EQ(result1[0], "a");
  EXPECT_EQ(result1[1], "b");
  EXPECT_EQ(result1[2], "c");

  std::vector<std::string> result2 = ::inip::Tools::split(",a,b,c,", ",", true);
  EXPECT_EQ(result2.size(), 3);
  EXPECT_EQ(result2[0], "a");
  EXPECT_EQ(result2[1], "b");
  EXPECT_EQ(result2[2], "c");
}

TEST(ToolsTest, TrimTest)
{
  EXPECT_EQ(::inip::Tools::trim("  hello  "), "hello");
  EXPECT_EQ(::inip::Tools::trim("hello"), "hello");
  EXPECT_EQ(::inip::Tools::trim(""), "");

  EXPECT_EQ(::inip::Tools::trim("  hello"), "hello");

  EXPECT_EQ(::inip::Tools::trim("hello  "), "hello");

  EXPECT_EQ(::inip::Tools::trim("\t\n hello \t\n"), "hello");

  EXPECT_EQ(::inip::Tools::trim("   \t\n  "), "");

  EXPECT_EQ(::inip::Tools::trim("  hello  world  "), "hello  world");

  EXPECT_EQ(::inip::Tools::trim("  测试  "), "测试");
}

TEST(ToolsTest, CoverMapTest)
{
  std::map<int, std::string> map1 = {{1, "one"}, {2, "two"}};
  std::map<int, std::string> map2 = {{2, "TWO"}, {3, "three"}};

  ::inip::Tools::cover_map(map1, map2);

  EXPECT_EQ(map1.size(), 3);
  EXPECT_EQ(map1[1], "one");
  EXPECT_EQ(map1[2], "TWO");
  EXPECT_EQ(map1[3], "three");

  std::map<std::string, int> map3 = {{"a", 1}, {"b", 2}};
  std::map<std::string, int> map4 = {{"b", 20}, {"c", 30}};

  ::inip::Tools::cover_map(map3, map4);

  EXPECT_EQ(map3.size(), 3);
  EXPECT_EQ(map3["a"], 1);
  EXPECT_EQ(map3["b"], 20);
  EXPECT_EQ(map3["c"], 30);

  std::map<int, double> map5 = {{1, 1.1}, {2, 2.2}};
  std::map<int, double> map6 = {};

  ::inip::Tools::cover_map(map5, map6);
  EXPECT_EQ(map5.size(), 2);
  EXPECT_EQ(map5[1], 1.1);
  EXPECT_EQ(map5[2], 2.2);

  std::map<int, double> map7 = {};
  std::map<int, double> map8 = {{1, 1.1}, {2, 2.2}};

  ::inip::Tools::cover_map(map7, map8);
  EXPECT_EQ(map7.size(), 2);
  EXPECT_EQ(map7[1], 1.1);
  EXPECT_EQ(map7[2], 2.2);
}

TEST(ToolsTest, AppendSecValueTest)
{
  std::vector<int> vec1 = {1, 2, 3};
  ::inip::Tools::append_sec_value(vec1, 4);
  EXPECT_EQ(vec1.size(), 4);
}

TEST(ToolsTest, AppendSecTest)
{
  std::vector<int> vec1 = {1, 2, 3};
  std::vector<int> vec2 = {3, 4, 5};

  ::inip::Tools::append_sec(vec1, vec2);
  EXPECT_EQ(vec1.size(), 5);
}

TEST(ToolsTest, SplitAndTrimCombinationTest)
{
  std::string csv_line = "  John , Doe , 30 , New York  ";
  std::vector<std::string> fields = ::inip::Tools::split(csv_line, ",");

  for (auto &field : fields) {
    field = ::inip::Tools::trim(field);
  }

  EXPECT_EQ(fields.size(), 4);
  EXPECT_EQ(fields[0], "John");
  EXPECT_EQ(fields[1], "Doe");
  EXPECT_EQ(fields[2], "30");
  EXPECT_EQ(fields[3], "New York");

  std::string config_line = "key = value";
  std::vector<std::string> parts = ::inip::Tools::split(config_line, "=");
  for (auto &part : parts) {
    part = ::inip::Tools::trim(part);
  }

  EXPECT_EQ(parts.size(), 2);
  EXPECT_EQ(parts[0], "key");
  EXPECT_EQ(parts[1], "value");
}

TEST(ToolsTest, PerformanceTest)
{
  const int num_segments = 1000;
  std::string long_string;
  for (int i = 0; i < num_segments; ++i) {
    long_string += "segment" + std::to_string(i);
    if (i < num_segments - 1) {
      long_string += ",";
    }
  }

  auto start = std::chrono::steady_clock::now();
  std::vector<std::string> result = ::inip::Tools::split(long_string, ",");
  auto end = std::chrono::steady_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  std::cout << "Split " << num_segments << " segments took " << duration.count() << " microseconds\n";

  EXPECT_EQ(result.size(), num_segments);
}

TEST(ToolsTest, EdgeCaseTests)
{
  {
    std::string huge(10000, 'a');
    huge += "," + std::string(10000, 'b');
    std::vector<std::string> result = ::inip::Tools::split(huge, ",");
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], std::string(10000, 'a'));
    EXPECT_EQ(result[1], std::string(10000, 'b'));
  }

  {
    std::vector<std::string> result = ::inip::Tools::split("测试★分割★功能", "★");
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "测试");
    EXPECT_EQ(result[1], "分割");
    EXPECT_EQ(result[2], "功能");
  }

  {
    std::string test_str = "a,b,c";
    std::vector<std::string> split_result = ::inip::Tools::split(test_str, ',');

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

TEST(ToolsTest, AddEscapeChars)
{
  std::string raw_str = "abc[], test #, t;est;==";
  std::string esp_str = ::inip::Tools::add_escape(raw_str);

  EXPECT_EQ(esp_str, "abc\\[], test \\#, t\\;est\\;\\=\\=");
}

TEST(ToolsTest, RemoveEscapeChars)
{
  std::string esp_str = "abc\\[], test \\#, t\\;est\\;\\=\\=";
  std::string raw_str = ::inip::Tools::remove_escape(esp_str);

  EXPECT_EQ(raw_str, "abc[], test #, t;est;==");
}
