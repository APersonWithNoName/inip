#include "test_inimgr.hpp"

#include "inip/IniMgr.hpp"

#include <gtest/gtest.h>
#include <sstream>

TEST_F(IniMgrTest, ConstructorAndBasic)
{
  inip::IniMgr ini("test.ini");
  SUCCEED();
}

TEST_F(IniMgrTest, LineFilter)
{
  inip::IniMgr ini("test.ini");

  std::string output;

  // empty
  EXPECT_FALSE(ini.line_filter("", output));
  EXPECT_EQ(output, "");

  // space only
  EXPECT_FALSE(ini.line_filter("   ", output));
  EXPECT_EQ(output, "");

  // comments
  EXPECT_FALSE(ini.line_filter("; comment", output));
  EXPECT_EQ(output, "");

  EXPECT_FALSE(ini.line_filter("# comment", output));
  EXPECT_EQ(output, "");

  // normal
  EXPECT_TRUE(ini.line_filter("key = value", output));
  EXPECT_EQ(output, "key = value");

  // normal + comment
  EXPECT_TRUE(ini.line_filter("key = value ; comment", output));
  EXPECT_EQ(output, "key = value");

  // escape char
  EXPECT_TRUE(ini.line_filter("key = value \\; not comment", output));
  EXPECT_EQ(output, "key = value \\; not comment");

  // front space
  EXPECT_TRUE(ini.line_filter("   key = value", output));
  EXPECT_EQ(output, "key = value");
}

TEST_F(IniMgrTest, Contains)
{
  std::string content =
    "[section1]\n"
    "key1 = value1\n"
    "[section2]\n"
    "key2 = value2\n";

  createTempFile(content);
  inip::IniMgr ini(temp_file_name);
  std::vector<inip::err::Errors> err_list;

  EXPECT_FALSE(ini.contains("section1"));

  auto result = ini.parse_file(err_list);
  EXPECT_EQ(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);

  EXPECT_TRUE(ini.contains("section1"));
  EXPECT_TRUE(ini.contains("section2"));
  EXPECT_FALSE(ini.contains("nonexistent"));
}

TEST_F(IniMgrTest, ParseStr)
{
  inip::IniMgr ini("test.ini");

  std::string content =
    "[database]\n"
    "host = localhost\n"
    "port = 3306\n"
    "\n"
    "[server]\n"
    "address = 127.0.0.1\n"
    "timeout = 30\n";

  std::istringstream iss(content);
  std::vector<inip::err::Errors> err_list;

  auto result = ini.parse_str(iss, err_list);
  EXPECT_EQ(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);
  EXPECT_TRUE(err_list.empty());
}

TEST_F(IniMgrTest, ParseFile)
{
  std::string content =
    "[user]\n"
    "name = John Doe\n"
    "email = john@example.com\n"
    "age = 30\n"
    "\n"
    "[settings]\n"
    "theme = dark\n"
    "notifications = true\n";

  createTempFile(content);
  inip::IniMgr ini(temp_file_name);
  std::vector<inip::err::Errors> err_list;

  auto result = ini.parse_file(err_list);
  EXPECT_EQ(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);
}

TEST_F(IniMgrTest, ParseErrorMissingBrackets)
{
  inip::IniMgr ini("test.ini");

  std::string content =
    "[section\n" // left  ]
    "key = value\n";

  std::istringstream iss(content);
  std::vector<inip::err::Errors> err_list;

  auto result = ini.parse_str(iss, err_list, false); // 不跳过失败

  EXPECT_NE(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);
  EXPECT_FALSE(err_list.empty());
}

TEST_F(IniMgrTest, ParseErrorEmptySection)
{
  inip::IniMgr ini("test.ini");

  std::string content =
    "[]\nkey = value\n";

  std::istringstream iss(content);
  std::vector<inip::err::Errors> err_list;

  auto result = ini.parse_str(iss, err_list, false);
  EXPECT_NE(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);
}

TEST_F(IniMgrTest, ParseErrorMissingEqual)
{
  inip::IniMgr ini("test.ini");

  std::string content =
    "[section]\n"
    "key value\n"
    "key2 = value2\n";

  std::istringstream iss(content);
  std::vector<inip::err::Errors> err_list;

  auto result = ini.parse_str(iss, err_list, false);
  EXPECT_NE(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);
}

TEST_F(IniMgrTest, ParseErrorEmptyKey)
{
  inip::IniMgr ini("test.ini");

  std::string content =
    "[section]\n"
    " = value\n"
    "key2 = value2\n";

  std::istringstream iss(content);
  std::vector<inip::err::Errors> err_list;

  auto result = ini.parse_str(iss, err_list, false);
  EXPECT_NE(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);
}

TEST_F(IniMgrTest, DuplicateSections)
{
  inip::IniMgr ini("test.ini");

  std::string content =
    "[section]\n"
    "key1 = value1\n"
    "[section]\n"
    "key2 = value2\n";

  std::istringstream iss(content);
  std::vector<inip::err::Errors> err_list;

  auto result = ini.parse_str(iss, err_list, false, true, true, false, true, true);
  EXPECT_NE(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);

  std::istringstream iss2(content);
  err_list.clear();

  result = ini.parse_str(iss2, err_list, false, false, false, true, false);
  EXPECT_EQ(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);
}

TEST_F(IniMgrTest, DuplicateKeys)
{
  inip::IniMgr ini("test.ini");
  inip::IniMgr ini2("test.ini");

  std::string content =
    "[section]\n"
    "key = value1\n"
    "key = value2\n";

  std::istringstream iss(content);
  std::vector<inip::err::Errors> err_list;

  auto result = ini.parse_str(iss, err_list, false, true, true, true, true, false, true);
  EXPECT_NE(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);

  std::istringstream iss2(content);
  err_list.clear();

  result = ini2.parse_str(iss2, err_list, false, false, false, false, false, false, false);
  EXPECT_EQ(result.get_code_err(), inip::err::ErrCode::KEY_EXISTS);
}

TEST_F(IniMgrTest, EscapedEqual)
{
  inip::IniMgr ini("test.ini");

  std::string content =
    "[section]\n"
    "key\\=with\\=equals = value\\=with\\=equals\n";

  std::istringstream iss(content);
  std::vector<inip::err::Errors> err_list;

  auto result = ini.parse_str(iss, err_list);
  EXPECT_EQ(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);
}

TEST_F(IniMgrTest, EqualAtPositionZero)
{
  inip::IniMgr ini("test.ini");

  std::string content =
    "[section]\n"
    "= value\n";

  std::istringstream iss(content);
  std::vector<inip::err::Errors> err_list;

  auto result = ini.parse_str(iss, err_list, false);
  EXPECT_NE(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);
}

TEST_F(IniMgrTest, GetLine)
{
  std::string content =
    "[section1]\n"
    "key1 = value1\n"
    "\n"
    "[section2]\n"
    "key2 = value2\n";

  createTempFile(content);
  inip::IniMgr ini(temp_file_name);
  std::vector<inip::err::Errors> err_list;

  auto result = ini.parse_file(err_list);
  EXPECT_EQ(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);

  EXPECT_EQ(ini.get_line("section1"), 1);
  EXPECT_EQ(ini.get_line("section2"), 4);

  EXPECT_EQ(ini.get_line("nonexistent"), 0);
  EXPECT_EQ(ini.get_line("section1", "nonexistent"), 0);
}

TEST_F(IniMgrTest, FileOpenError)
{
  inip::IniMgr ini("nonexistent_file.ini");
  std::vector<inip::err::Errors> err_list;

  auto result = ini.parse_file(err_list);
  EXPECT_EQ(result.get_code_err(), inip::err::ErrCode::CANNOT_OPEN_FILE);
}

TEST_F(IniMgrTest, EscapedComment)
{
  inip::IniMgr ini("test.ini");

  std::string content =
    "[section]\n"
    "key = value \\; not a comment ; this is a comment\n"
    "key2 = value2 # real comment\n";

  std::istringstream iss(content);
  std::vector<inip::err::Errors> err_list;

  auto result = ini.parse_str(iss, err_list);
  EXPECT_EQ(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);
}

TEST_F(IniMgrTest, ComplexIniFile)
{
  std::string content =
    "; This is a comment\n"
    "# This is also a comment\n"
    "\n"
    "[database]\n"
    "host = localhost\n"
    "port = 3306\n"
    "user = root\n"
    "password = secret\\#password ; escaped #\n"
    "\n"
    "[server]\n"
    "  address = 127.0.0.1  ; with spaces\n"
    "timeout=60\n" // no spaces
    "enabled = true\n";

  createTempFile(content);
  inip::IniMgr ini(temp_file_name);
  std::vector<inip::err::Errors> err_list;

  auto result = ini.parse_file(err_list);
  EXPECT_EQ(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);

  EXPECT_TRUE(ini.contains("database"));
  EXPECT_TRUE(ini.contains("server"));
}

TEST_F(IniMgrTest, FailedSkipOption)
{
  inip::IniMgr ini("test.ini");

  std::string content =
    "[section1]\n"
    "key1 = value1\n"
    "[section2\n" // miss ]
    "key2 = value2\n"
    "[section3]\n"
    "key3 = value3\n";

  std::istringstream iss(content);
  std::vector<inip::err::Errors> err_list;

  auto result = ini.parse_str(iss, err_list, false);
  EXPECT_NE(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);

  EXPECT_FALSE(ini.contains("section3"));

  std::istringstream iss2(content);
  err_list.clear();

  inip::IniMgr ini2("test.ini");
  result = ini2.parse_str(iss2, err_list, true); // skip failure

  EXPECT_EQ(result.get_code_err(), inip::err::ErrCode::MISS_RIGHT_BRACKETS);
  EXPECT_TRUE(ini2.contains("section1"));
  EXPECT_FALSE(ini2.contains("section2"));
  EXPECT_TRUE(ini2.contains("section3"));
}

TEST_F(IniMgrTest, ParseLongStr)
{
  inip::IniMgr ini("test.ini");

  std::string content =
    "[database]\n"
    "host = localhost\n"
    "desc = this is \\\n"
    "  a serv\\\n"
    "er\n"
    ""
    "port = 3306\n"
    "\n";

  std::istringstream iss(content);
  std::vector<inip::err::Errors> err_list;

  auto result = ini.parse_str(iss, err_list);
  EXPECT_EQ(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);

  EXPECT_EQ(ini.get_data()->at("database").get_value("desc"), "this is   a server");
}


TEST_F(IniMgrTest, ParseEscapeChar)
{
  inip::IniMgr ini("test.ini");

  std::string content =
    "[database]\n"
    "ho\\=st \\== local\\;host\n"
    "desc = this is \\\n"
    "  a serv\\\n"
    "er\n"
    ""
    "port = 3306\n"
    "\n";

  std::istringstream iss(content);
  std::vector<inip::err::Errors> err_list;

  auto result = ini.parse_str(iss, err_list);
  EXPECT_EQ(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);

  EXPECT_EQ(ini.get_data()->at("database").get_value("desc"), "this is   a server");
  EXPECT_EQ(ini.get_data()->at("database").get_value("ho=st ="), "local;host");
}
