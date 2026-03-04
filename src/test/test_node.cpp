#include <gtest/gtest.h>
#include <string>

#include "inip/Node.hpp"

TEST(NodeTest, DefaultConstructor)
{
  inip::Node node;

  EXPECT_EQ(node.get_value(), "");
  EXPECT_EQ(node.get_line(), 0);
  EXPECT_EQ(node.to_string(), "Node{value=\"\", line=0}");
}

TEST(NodeTest, ValueConstructor)
{
  inip::Node node("test_value");

  EXPECT_EQ(node.get_value(), "test_value");
  EXPECT_EQ(node.get_line(), 0);
  EXPECT_EQ(node.to_string(), "Node{value=\"test_value\", line=0}");
}

TEST(NodeTest, CopyConstructor)
{
  inip::Node original("original_value");
  original.set_line(42);

  inip::Node copy(original);

  EXPECT_EQ(copy.get_value(), "original_value");
  EXPECT_EQ(copy.get_line(), 42);
  EXPECT_EQ(original.get_value(), "original_value"); // 确保原对象未被修改
  EXPECT_EQ(original.get_line(), 42);
}

TEST(NodeTest, SetValue)
{
  inip::Node node;

  node.set_value("new_value");
  EXPECT_EQ(node.get_value(), "new_value");

  node.set_value("");
  EXPECT_EQ(node.get_value(), "");

  node.set_value("value_with_spaces");
  EXPECT_EQ(node.get_value(), "value_with_spaces");
}

TEST(NodeTest, SetLine)
{
  inip::Node node;

  node.set_line(10);
  EXPECT_EQ(node.get_line(), 10);

  node.set_line(0);
  EXPECT_EQ(node.get_line(), 0);

  node.set_line(1000);
  EXPECT_EQ(node.get_line(), 1000);

  // 测试最大行号(如果有定义的话)
  node.set_line(std::numeric_limits<inip::Types::LineNum>::max());
  EXPECT_EQ(node.get_line(), std::numeric_limits<inip::Types::LineNum>::max());
}

TEST(NodeTest, GetValue)
{
  inip::Node node1;
  EXPECT_EQ(node1.get_value(), "");

  inip::Node node2("test");
  EXPECT_EQ(node2.get_value(), "test");

  // const 版本测试
  const inip::Node const_node("const_test");
  EXPECT_EQ(const_node.get_value(), "const_test");
}

TEST(NodeTest, GetLine)
{
  inip::Node node;
  EXPECT_EQ(node.get_line(), 0);

  node.set_line(123);
  EXPECT_EQ(node.get_line(), 123);

  // const 版本测试
  const inip::Node const_node;
  EXPECT_EQ(const_node.get_line(), 0);
}

TEST(NodeTest, ToString)
{
  inip::Node node1;
  EXPECT_EQ(node1.to_string(), "Node{value=\"\", line=0}");

  inip::Node node2("test_value");
  EXPECT_EQ(node2.to_string(), "Node{value=\"test_value\", line=0}");

  inip::Node node3;
  node3.set_value("value_with_quotes");
  node3.set_line(99);
  EXPECT_EQ(node3.to_string(), "Node{value=\"value_with_quotes\", line=99}");

  // 测试特殊字符
  inip::Node node4("value\nwith\t特殊字符");
  EXPECT_EQ(node4.to_string(), "Node{value=\"value\nwith\t特殊字符\", line=0}");
}

TEST(NodeTest, StringAssignmentOperator)
{
  inip::Node node;

  node = "assigned_value";
  EXPECT_EQ(node.get_value(), "assigned_value");
  EXPECT_EQ(node.get_line(), 0); // 行号应该保持不变

  // 链式赋值测试
  inip::Node node2;
  node2 = node = "chained_value";
  EXPECT_EQ(node.get_value(), "chained_value");
  EXPECT_EQ(node2.get_value(), "chained_value");
}

TEST(NodeTest, NodeAssignmentOperator)
{
  inip::Node node1("value1");
  node1.set_line(10);

  inip::Node node2;
  node2 = node1;

  EXPECT_EQ(node2.get_value(), "value1");
  EXPECT_EQ(node2.get_line(), 10);

  // 测试自赋值
  node2 = node2;
  EXPECT_EQ(node2.get_value(), "value1");
  EXPECT_EQ(node2.get_line(), 10);

  // 链式赋值
  inip::Node node3, node4;
  node4 = node3 = node1;
  EXPECT_EQ(node3.get_value(), "value1");
  EXPECT_EQ(node3.get_line(), 10);
  EXPECT_EQ(node4.get_value(), "value1");
  EXPECT_EQ(node4.get_line(), 10);
}

TEST(NodeTest, MixedOperations)
{
  inip::Node node;

  // 设置值和行号
  node.set_value("initial");
  node.set_line(5);
  EXPECT_EQ(node.get_value(), "initial");
  EXPECT_EQ(node.get_line(), 5);

  // 使用字符串赋值运算符
  node = "updated";
  EXPECT_EQ(node.get_value(), "updated");
  EXPECT_EQ(node.get_line(), 5); // 行号应该保持不变

  // 复制到新对象
  inip::Node other("other");
  other.set_line(20);
  node = other;
  EXPECT_EQ(node.get_value(), "other");
  EXPECT_EQ(node.get_line(), 20);

  // 再次使用字符串赋值
  node = "final";
  EXPECT_EQ(node.get_value(), "final");
  EXPECT_EQ(node.get_line(), 20); // 行号仍然保持不变
}

TEST(NodeTest, EdgeCases)
{
  // 空字符串值
  inip::Node empty_value_node("");
  EXPECT_EQ(empty_value_node.get_value(), "");
  EXPECT_EQ(empty_value_node.to_string(), "Node{value=\"\", line=0}");

  // 长字符串值
  std::string long_str(1000, 'x');
  inip::Node long_node(long_str);
  EXPECT_EQ(long_node.get_value(), long_str);

  // 行号为0
  inip::Node zero_line_node;
  zero_line_node.set_line(0);
  EXPECT_EQ(zero_line_node.get_line(), 0);

  // 负行号(如果LineNum是有符号类型)
  // 这取决于inip::Types::LineNum的类型定义
  // 如果允许负数：
  // inip::Node negative_line_node;
  // negative_line_node.set_line(-1);
  // EXPECT_EQ(negative_line_node.get_line(), -1);
}

TEST(NodeTest, ConstCorrectness)
{
  const inip::Node const_node("const_value");

  // 这些应该能编译通过(const方法)
  std::string value = const_node.get_value();
  inip::Types::LineNum line = const_node.get_line();
  std::string str = const_node.to_string();

  EXPECT_EQ(value, "const_value");
  EXPECT_EQ(line, 0);
  EXPECT_EQ(str, "Node{value=\"const_value\", line=0}");

  // 这些不应该编译通过(非const方法)
  // const_node.set_value("new_value");  // 编译错误
  // const_node.set_line(1);             // 编译错误
  // const_node = "new_string";          // 编译错误
}

TEST(NodeTest, AssignmentReturnValue)
{
  inip::Node node1, node2, node3;

  // 测试字符串赋值运算符的返回值
  inip::Node &ref1 = (node1 = "value1");
  EXPECT_EQ(&ref1, &node1); // 应该返回自身的引用
  EXPECT_EQ(node1.get_value(), "value1");

  // 测试节点赋值运算符的返回值
  inip::Node other("other_value");
  other.set_line(30);

  inip::Node &ref2 = (node2 = other);
  EXPECT_EQ(&ref2, &node2); // 应该返回自身的引用
  EXPECT_EQ(node2.get_value(), "other_value");
  EXPECT_EQ(node2.get_line(), 30);

  // 链式赋值
  node3 = node2 = "chained";
  EXPECT_EQ(node2.get_value(), "chained");
  EXPECT_EQ(node3.get_value(), "chained");
}
