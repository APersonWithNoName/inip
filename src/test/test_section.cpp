#include <algorithm>
#include <gtest/gtest.h>
#include <list>
#include <map>
#include <string>
#include <vector>

#include "inip/Exception.hpp"
#include "inip/Node.hpp"
#include "inip/Section.hpp"

TEST(SectionTest, DefaultConstructor)
{
  inip::Section section;

  EXPECT_EQ(section.get_name(), "");
  EXPECT_EQ(section.get_line(), 0);
  EXPECT_TRUE(section.empty());
  EXPECT_EQ(section.size(), 0);
  EXPECT_EQ(section.to_string(), "");
}

TEST(SectionTest, ParameterizedConstructorWithNameOnly)
{
  inip::Section section("TestSection");

  EXPECT_EQ(section.get_name(), "TestSection");
  EXPECT_EQ(section.get_line(), 0);
  EXPECT_TRUE(section.empty());
  EXPECT_EQ(section.size(), 0);
  EXPECT_EQ(section.to_string(), "[TestSection]\n");
}

TEST(SectionTest, ParameterizedConstructorWithData)
{
  std::map<std::string, inip::Node> data = {
    {"key1", inip::Node("value1")},
    {"key2", inip::Node("value2")}};

  inip::Section section("TestSection", data);

  EXPECT_EQ(section.get_name(), "TestSection");
  EXPECT_EQ(section.get_line(), 0);
  EXPECT_FALSE(section.empty());
  EXPECT_EQ(section.size(), 2);
  EXPECT_TRUE(section.contains("key1"));
  EXPECT_TRUE(section.contains("key2"));
  EXPECT_FALSE(section.contains("nonexistent"));
}

TEST(SectionTest, SetAndGetName)
{
  inip::Section section;

  section.set_name("NewSectionName");
  EXPECT_EQ(section.get_name(), "NewSectionName");

  section.set_name("");
  EXPECT_EQ(section.get_name(), "");
}

TEST(SectionTest, SetAndGetLine)
{
  inip::Section section;

  EXPECT_EQ(section.get_line(), 0);

  section.set_line(42);
  EXPECT_EQ(section.get_line(), 42);

  section.set_line(0);
  EXPECT_EQ(section.get_line(), 0);
}

TEST(SectionTest, SetMethodBasic)
{
  inip::Section section;

  section.set("key1", "value1");
  EXPECT_TRUE(section.contains("key1"));
  EXPECT_EQ(section.get_value("key1"), "value1");

  section.set("key1", "new_value");
  EXPECT_EQ(section.get_value("key1"), "new_value");

  section.set("key2", "value2");
  section.set("key3", "value3");
  EXPECT_EQ(section.size(), 3);
}

TEST(SectionTest, SetMethodWithLine)
{
  inip::Section section;

  section.set("key1", "value1", 10);
  EXPECT_TRUE(section.contains("key1"));
  EXPECT_EQ(section.get_value("key1"), "value1");

  inip::Node node = section.get_node("key1");
  EXPECT_EQ(node.get_line(), 10);

  section.set("key1", "new_value", 20);
  EXPECT_EQ(section.get_value("key1"), "new_value");
  node = section.get_node("key1");
  EXPECT_EQ(node.get_line(), 20);
}

TEST(SectionTest, InsertMethod)
{
  inip::Section section;

  inip::Node node1("value1");
  node1.set_line(5);

  inip::Node node2("value2");
  node2.set_line(10);

  section.insert("key1", node1);
  section.insert("key2", node2);

  EXPECT_TRUE(section.contains("key1"));
  EXPECT_TRUE(section.contains("key2"));
  EXPECT_EQ(section.size(), 2);

  inip::Node retrieved1 = section.get_node("key1");
  EXPECT_EQ(retrieved1.get_value(), "value1");
  EXPECT_EQ(retrieved1.get_line(), 5);

  inip::Node node3("value3");
  node3.set_line(15);
  section.insert("key1", node3);
  EXPECT_EQ(section.get_value("key1"), "value3");
  EXPECT_EQ(section.get_node("key1").get_line(), 15);
}

TEST(SectionTest, ContainsMethod)
{
  inip::Section section;

  EXPECT_FALSE(section.contains("key1"));

  section.set("key1", "value1");
  EXPECT_TRUE(section.contains("key1"));
  EXPECT_FALSE(section.contains("key2"));

  section.erase("key1");
  EXPECT_FALSE(section.contains("key1"));
}

TEST(SectionTest, GetNodeMethod)
{
  inip::Section section;
  section.set("key1", "value1");
  section.set("key2", "value2", 5);

  inip::Node node1 = section.get_node("key1");
  EXPECT_EQ(node1.get_value(), "value1");
  EXPECT_EQ(node1.get_line(), 0);

  inip::Node node2 = section.get_node("key2");
  EXPECT_EQ(node2.get_value(), "value2");
  EXPECT_EQ(node2.get_line(), 5);

  EXPECT_THROW(section.get_node("nonexistent"), inip::err::Errors);
}

TEST(SectionTest, GetValueMethod)
{
  inip::Section section;
  section.set("key1", "value1");
  section.set("key2", "value2");

  EXPECT_EQ(section.get_value("key1"), "value1");
  EXPECT_EQ(section.get_value("key2"), "value2");

  EXPECT_THROW(section.get_value("nonexistent"), inip::err::Errors);
}

TEST(SectionTest, GetValueDefMethod)
{
  inip::Section section;
  section.set("key1", "value1");

  EXPECT_EQ(section.get_value_def("key1", "default"), "value1");

  EXPECT_EQ(section.get_value_def("nonexistent", "default_value"), "default_value");

  EXPECT_EQ(section.get_value_def("nonexistent", ""), "");
}

TEST(SectionTest, AtMethod)
{
  inip::Section section;
  section.set("key1", "value1");
  section.set("key2", "value2", 10);

  inip::Node &node1 = section.at("key1");
  EXPECT_EQ(node1.get_value(), "value1");

  node1.set_value("modified_value");
  EXPECT_EQ(section.get_value("key1"), "modified_value");

  node1.set_line(15);
  EXPECT_EQ(section.get_node("key1").get_line(), 15);

  const inip::Section &const_section = section;
  const inip::Node &node2 = const_section.at("key2");
  EXPECT_EQ(node2.get_value(), "value2");
  EXPECT_EQ(node2.get_line(), 10);

  EXPECT_THROW(section.at("nonexistent"), inip::err::Errors);
  EXPECT_THROW(const_section.at("nonexistent"), inip::err::Errors);
}

TEST(SectionTest, SubscriptOperator)
{
  inip::Section section;

  inip::Node &node1 = section["new_key"];
  EXPECT_TRUE(section.contains("new_key"));
  EXPECT_EQ(node1.get_value(), "");

  node1 = "assigned_value";
  node1.set_line(5);
  EXPECT_EQ(section.get_value("new_key"), "assigned_value");
  EXPECT_EQ(section.get_node("new_key").get_line(), 5);

  section.set("existing_key", "existing_value");
  inip::Node &node2 = section["existing_key"];
  EXPECT_EQ(node2.get_value(), "existing_value");

  node2 = "updated_value";
  EXPECT_EQ(section.get_value("existing_key"), "updated_value");
}

TEST(SectionTest, EraseMethod)
{
  inip::Section section;
  section.set("key1", "value1");
  section.set("key2", "value2");
  section.set("key3", "value3");

  EXPECT_EQ(section.size(), 3);
  EXPECT_TRUE(section.contains("key2"));

  section.erase("key2");
  EXPECT_EQ(section.size(), 2);
  EXPECT_FALSE(section.contains("key2"));
  EXPECT_TRUE(section.contains("key1"));
  EXPECT_TRUE(section.contains("key3"));

  section.erase("nonexistent");
  EXPECT_EQ(section.size(), 2);

  section.erase("key1");
  section.erase("key3");
  EXPECT_TRUE(section.empty());
}

TEST(SectionTest, ClearMethod)
{
  inip::Section section("TestSection");
  section.set_line(5);
  section.set("key1", "value1");
  section.set("key2", "value2");

  EXPECT_FALSE(section.empty());
  EXPECT_EQ(section.get_name(), "TestSection");
  EXPECT_EQ(section.get_line(), 5);

  section.clear(false);
  EXPECT_TRUE(section.empty());
  EXPECT_EQ(section.size(), 0);
  EXPECT_EQ(section.get_name(), "TestSection");
  EXPECT_EQ(section.get_line(), 5);

  section.set("new_key", "new_value");

  section.clear(true);
  EXPECT_TRUE(section.empty());
  EXPECT_EQ(section.get_name(), "");
  EXPECT_EQ(section.get_line(), 0);
}

TEST(SectionTest, KeysMethod)
{
  inip::Section section;
  section.set("key1", "value1");
  section.set("key2", "value2");
  section.set("key3", "value3");

  std::vector<std::string> keys = section.keys();
  EXPECT_EQ(keys.size(), 3);

  EXPECT_TRUE(std::find(keys.begin(), keys.end(), "key1") != keys.end());
  EXPECT_TRUE(std::find(keys.begin(), keys.end(), "key2") != keys.end());
  EXPECT_TRUE(std::find(keys.begin(), keys.end(), "key3") != keys.end());

  inip::Section empty_section;
  std::vector<std::string> empty_keys = empty_section.keys();
  EXPECT_TRUE(empty_keys.empty());
}

TEST(SectionTest, KeysTemplateMethod)
{
  inip::Section section;
  section.set("key1", "value1");
  section.set("key2", "value2");

  auto vector_keys = section.keys<std::vector<std::string>>();
  EXPECT_EQ(vector_keys.size(), 2);

  auto list_keys = section.keys<std::list<std::string>>();
  EXPECT_EQ(list_keys.size(), 2);
}

TEST(SectionTest, ToStringMethod)
{
  inip::Section section("TestSection");
  section.set("key1", "value1");
  section.set("key2", "value2");
  section.set("key3", "value3=with=equals");

  std::string result = section.to_string();
  EXPECT_TRUE(result.find("[TestSection]") != std::string::npos);
  EXPECT_TRUE(result.find("key1 = value1") != std::string::npos);
  EXPECT_TRUE(result.find("key2 = value2") != std::string::npos);
  EXPECT_TRUE(result.find("key3 = value3=with=equals") != std::string::npos);

  inip::Section empty_named_section("EmptySection");
  EXPECT_EQ(empty_named_section.to_string(), "[EmptySection]\n");

  inip::Section empty_section;
  EXPECT_EQ(empty_section.to_string(), "");
}

TEST(SectionTest, Iterators)
{
  inip::Section section;
  section.set("key1", "value1");
  section.set("key2", "value2");
  section.set("key3", "value3");

  std::size_t count = 0;
  for (auto it = section.begin(); it != section.end(); ++it) {
    ++count;
    EXPECT_FALSE(it->first.empty());
    EXPECT_FALSE(it->second.get_value().empty());
  }
  EXPECT_EQ(count, 3);

  const inip::Section &const_section = section;
  count = 0;
  for (auto it = const_section.cbegin(); it != const_section.cend(); ++it) {
    ++count;
  }
  EXPECT_EQ(count, 3);

  count = 0;
  for (auto it = const_section.cbegin(); it != const_section.cend(); ++it) {
    ++count;
  }
  EXPECT_EQ(count, 3);

  count = 0;
  for (const auto &pair : section) {
    ++count;
  }
  EXPECT_EQ(count, 3);
}

TEST(SectionTest, ConstCorrectness)
{
  inip::Section mutable_section;
  mutable_section.set_name("TestSection");
  mutable_section.set_line(10);
  mutable_section.set("key1", "value1");

  const inip::Section &const_section = mutable_section;

  EXPECT_EQ(const_section.get_name(), "TestSection");
  EXPECT_EQ(const_section.get_line(), 10);
  EXPECT_EQ(const_section.size(), 1);
  EXPECT_FALSE(const_section.empty());
  EXPECT_TRUE(const_section.contains("key1"));
  EXPECT_EQ(const_section.get_value("key1"), "value1");
  EXPECT_EQ(const_section.get_value_def("key1", "default"), "value1");
  EXPECT_EQ(const_section.get_value_def("nonexistent", "default"), "default");

  const inip::Node &node = const_section.at("key1");
  EXPECT_EQ(node.get_value(), "value1");

  for (auto it = const_section.cbegin(); it != const_section.cend(); ++it) {
  }
}

TEST(SectionTest, MixedOperations)
{
  inip::Section section("MixedSection");

  section.set("key1", "value1");
  section.set("key2", "value2", 20);

  inip::Node custom_node("custom_value");
  custom_node.set_line(30);
  section.insert("key3", custom_node);

  section["key4"] = "assigned_value";

  EXPECT_EQ(section.size(), 4);
  EXPECT_EQ(section.get_value("key1"), "value1");
  EXPECT_EQ(section.get_node("key2").get_line(), 20);
  EXPECT_EQ(section.get_value("key3"), "custom_value");
  EXPECT_EQ(section.get_node("key3").get_line(), 30);
  EXPECT_EQ(section.get_value("key4"), "assigned_value");

  section.at("key1") = "modified_value";
  EXPECT_EQ(section.get_value("key1"), "modified_value");

  section.at("key4").set_line(40);
  EXPECT_EQ(section.get_node("key4").get_line(), 40);

  section.erase("key2");
  section.erase("key3");
  EXPECT_EQ(section.size(), 2);

  section.clear();
  EXPECT_TRUE(section.empty());
}

TEST(SectionTest, EdgeCases)
{
  inip::Section section;

  section.set("", "empty_key_value");
  EXPECT_TRUE(section.contains(""));
  EXPECT_EQ(section.get_value(""), "empty_key_value");

  section.set("empty_value_key", "");
  EXPECT_EQ(section.get_value("empty_value_key"), "");

  section.set("key=with=equals", "value=with=equals");
  section.set("key\nwith\nnewline", "value\nwith\nnewline");
  section.set("key\twith\ttab", "value\twith\ttab");

  std::string long_key(1000, 'x');
  std::string long_value(1000, 'y');
  section.set(long_key, long_value);
  EXPECT_EQ(section.get_value(long_key), long_value);

  EXPECT_GT(section.max_size(), 0);
}

TEST(SectionTest, CopyConstructor)
{
  inip::Section original("OriginalSection");
  original.set_line(5);
  original.set("key1", "value1");
  original.set("key2", "value2", 10);

  inip::Section copy(original);
  EXPECT_EQ(copy.get_name(), "OriginalSection");
  EXPECT_EQ(copy.get_line(), 5);
  EXPECT_EQ(copy.size(), 2);
  EXPECT_EQ(copy.get_value("key1"), "value1");
  EXPECT_EQ(copy.get_node("key2").get_line(), 10);

  copy.set("key1", "modified_value");
  EXPECT_EQ(original.get_value("key1"), "value1");
  EXPECT_EQ(copy.get_value("key1"), "modified_value");
}

TEST(SectionTest, AssignmentOperator)
{
  inip::Section original("OriginalSection");
  original.set_line(5);
  original.set("key1", "value1");
  original.set("key2", "value2", 10);

  inip::Section copy;
  copy = original;
  EXPECT_EQ(copy.get_name(), "OriginalSection");
  EXPECT_EQ(copy.get_line(), 5);
  EXPECT_EQ(copy.size(), 2);

  copy.set("key1", "modified_value");
  EXPECT_EQ(original.get_value("key1"), "value1");
  EXPECT_EQ(copy.get_value("key1"), "modified_value");

  copy = copy;
  EXPECT_EQ(copy.get_value("key1"), "modified_value");
  EXPECT_EQ(copy.size(), 2);
}
