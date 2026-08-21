#include "test_ini.hpp"

#include "inip/inip.hpp"

TEST_F(IniTest, Constructor)
{
  inip::ini ini1;
  SUCCEED();

  // with name
  inip::ini ini2("test.ini");
  SUCCEED();
}

TEST_F(IniTest, LoadFile)
{
  std::vector<inip::err::Errors> err_list;
  std::string content =
    "[database]\n"
    "host = localhost\n"
    "port = 3306\n"
    "\n"
    "[server]\n"
    "address = 127.0.0.1\n";

  createTempFile(content);
  inip::ini ini(temp_file_name);

  auto result = ini.load_file(err_list);
  EXPECT_EQ(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);
}

TEST_F(IniTest, LoadFileWithDifferentName)
{
  std::vector<inip::err::Errors> err_list;
  std::string content =
    "[test]\n"
    "key = value\n";

  createTempFile(content);
  inip::ini ini;

  auto result = ini.load_file(temp_file_name, err_list);
  EXPECT_EQ(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);
}

TEST_F(IniTest, LoadFileWithNotExsistFile)
{
  std::vector<inip::err::Errors> err_list;
  inip::ini ini;

  auto result = ini.load_file("NotExistFile.ini", err_list);
  EXPECT_EQ(result.get_code_err(), inip::err::ErrCode::CANNOT_OPEN_FILE);
  EXPECT_FALSE(err_list.empty());
}

TEST_F(IniTest, LoadStr)
{
  inip::ini ini;

  std::vector<inip::err::Errors> err_list;
  std::string content =
    "[section1]\n"
    "key1 = value1\n"
    "[section2]\n"
    "key2 = value2\n";

  auto result = ini.load_str(content, err_list);
  EXPECT_EQ(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);
}

TEST_F(IniTest, LoadSStr)
{
  inip::ini ini;

  std::vector<inip::err::Errors> err_list;
  std::stringstream ss;
  ss << "[settings]\n"
     << "theme = dark\n"
     << "notifications = true\n";

  auto result = ini.load_sstr(ss, err_list);
  EXPECT_EQ(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);
}

TEST_F(IniTest, Contains)
{
  inip::ini ini;

  std::vector<inip::err::Errors> err_list;
  std::string content =
    "[database]\n"
    "host = localhost\n"
    "[server]\n"
    "address = 127.0.0.1\n";

  ini.load_str(content, err_list);

  EXPECT_TRUE(ini.contains("database"));
  EXPECT_TRUE(ini.contains("server"));
  EXPECT_FALSE(ini.contains("nonexistent"));

  EXPECT_TRUE(ini.contains(""));
}

TEST_F(IniTest, ExistKey)
{
  inip::ini ini;

  std::vector<inip::err::Errors> err_list;
  std::string content =
    "global_key = global_value\n"
    "[database]\n"
    "host = localhost\n"
    "port = 3306\n"
    "\n";

  ini.load_str(content, err_list);

  EXPECT_TRUE(ini.exist_key("database", "host"));
  EXPECT_TRUE(ini.exist_key("database", "port"));
  EXPECT_FALSE(ini.exist_key("database", "nonexistent"));

  EXPECT_TRUE(ini.exist_key("global_key"));
  EXPECT_FALSE(ini.exist_key("nonexistent_key"));

  EXPECT_FALSE(ini.exist_key("nonexistent", "key"));
}

TEST_F(IniTest, SetFunctions)
{
  inip::ini ini;

  ini.set("new_section");
  EXPECT_TRUE(ini.contains("new_section"));

  ini.set("global_key", "global_value");
  EXPECT_TRUE(ini.exist_key("global_key"));

  ini.set("new_section", "key1", "value1");
  EXPECT_TRUE(ini.exist_key("new_section", "key1"));

  EXPECT_EQ(ini.get_section("new_section").get_name(), "new_section");
}

TEST_F(IniTest, Get)
{
  inip::ini ini;

  std::vector<inip::err::Errors> err_list;
  std::string content =
    "\n"
    "global = value\n"
    "[database]\n"
    "host = localhost\n"
    "port = 3306\n"
    "\n";

  ini.load_str(content, err_list);

  EXPECT_EQ(ini.get("database", "host"), "localhost");
  EXPECT_EQ(ini.get("database", "port"), "3306");

  EXPECT_EQ(ini.get("global"), "value");

  EXPECT_THROW(ini.get("database", "nonexistent"), inip::err::Errors);
  EXPECT_THROW(ini.get("nonexistent", "key"), inip::err::Errors);
}

TEST_F(IniTest, GetDefault)
{
  inip::ini ini;

  std::vector<inip::err::Errors> err_list;
  std::string content =
    "[section]\n"
    "existing = value\n";

  ini.load_str(content, err_list);

  EXPECT_EQ(ini.get_default("section", "existing", "default"), "value");

  // return default value
  EXPECT_EQ(ini.get_default("section", "nonexistent", "default_value"), "default_value");
  EXPECT_EQ(ini.get_default("nonexistent", "key", "default"), "default");
}

TEST_F(IniTest, Clear)
{
  inip::ini ini;

  std::vector<inip::err::Errors> err_list;
  std::string content =
    "[section1]\n"
    "key1 = value1\n"
    "[section2]\n"
    "key2 = value2\n";

  ini.load_str(content, err_list);

  EXPECT_TRUE(ini.contains("section1"));
  EXPECT_TRUE(ini.contains("section2"));

  ini.clear();
  err_list.clear();

  // without any sections
  EXPECT_FALSE(ini.contains("section1"));
  EXPECT_FALSE(ini.contains("section2"));

  /// @TODO: add public interfaces and test clear_nameme
}

TEST_F(IniTest, ToString)
{
  inip::ini ini;

  std::vector<inip::err::Errors> err_list;
  std::string content =
    "[database]\n"
    "host = localhost\n"
    "port = 3306\n"
    "\n"
    "[server]\n"
    "address = 127.0.0.1\n";

  ini.load_str(content, err_list);

  std::string result = ini.to_string();

  EXPECT_NE(result.find("[database]"), std::string::npos);
  EXPECT_NE(result.find("host = localhost"), std::string::npos);
  EXPECT_NE(result.find("[server]"), std::string::npos);
  EXPECT_NE(result.find("address = 127.0.0.1"), std::string::npos);
}

TEST_F(IniTest, Write)
{
  inip::ini ini;

  std::vector<inip::err::Errors> err_list;
  std::string content =
    "[test]\n"
    "key1 = value1\n"
    "key2 = value2\n";

  ini.load_str(content, err_list);

  std::string output_file = "test_output.ini";
  ini.write(output_file);

  std::ifstream file(output_file);
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string file_content = buffer.str();

  EXPECT_NE(file_content.find("[test]"), std::string::npos);
  EXPECT_NE(file_content.find("key1 = value1"), std::string::npos);
  EXPECT_NE(file_content.find("key2 = value2"), std::string::npos);

  file.close();
  std::remove(output_file.c_str());
}

TEST_F(IniTest, WriteDefault)
{
  std::string content =
    "[test]\n"
    "key = value\n";

  createTempFile(content);
  inip::ini ini(temp_file_name);

  ini.set("test", "new_key", "new_value");

  EXPECT_NO_THROW(ini.write());

  std::remove(temp_file_name.c_str());
}

TEST_F(IniTest, OperatorBracket)
{
  inip::ini ini;

  inip::Section &section = ini["new_section"];
  EXPECT_TRUE(ini.contains("new_section"));

  section.set("key", "value");
  EXPECT_TRUE(ini.exist_key("new_section", "key"));
  EXPECT_EQ(ini.get("new_section", "key"), "value");
}

TEST_F(IniTest, At)
{
  inip::ini ini;

  std::vector<inip::err::Errors> err_list;
  std::string content =
    "[existing]\n"
    "key = value\n";

  ini.load_str(content, err_list);

  inip::Section &section = ini.at("existing");
  EXPECT_EQ(section.get_value("key"), "value");

  section.set("key", "new_value");
  EXPECT_EQ(ini.get("existing", "key"), "new_value");

  EXPECT_THROW(ini.at("nonexistent"), inip::err::Errors);
}

TEST_F(IniTest, SizeFunctions)
{
  inip::ini ini;

  std::size_t initial_size = ini.size();

  ini.set("section1");
  ini.set("section2");

  EXPECT_EQ(ini.size(), initial_size + 2);

  EXPECT_GT(ini.max_size(), 0);
}

TEST_F(IniTest, BoolFunctions)
{
  inip::ini ini;

  ini.set_bool("section", "enabled", true);
  ini.set_bool("section", "debug", false);

  EXPECT_TRUE(ini.get_bool("section", "enabled"));
  EXPECT_FALSE(ini.get_bool("section", "debug"));

  EXPECT_TRUE(ini.get_bool_def("section", "nonexistent", true));
  EXPECT_FALSE(ini.get_bool_def("section", "nonexistent", false));
}

TEST_F(IniTest, IntFunctions)
{
  inip::ini ini;

  ini.set_int("section", "count", 42);
  ini.set_int("section", "negative", -10);

  EXPECT_EQ(ini.get_int("section", "count"), 42);
  EXPECT_EQ(ini.get_int("section", "negative"), -10);

  EXPECT_EQ(ini.get_int_def("section", "nonexistent", 99), 99);
}

TEST_F(IniTest, FloatFunctions)
{
  inip::ini ini;

  ini.set_float("section", "pi", 3.14159f);
  ini.set_double("section", "precise", 3.14159265358979);

  EXPECT_FLOAT_EQ(ini.get_float("section", "pi"), 3.14159f);
  EXPECT_NEAR(ini.get_double("section", "precise"), 3.14159265358979, 1e-6);

  EXPECT_FLOAT_EQ(ini.get_float_def("section", "nonexistent", 2.718f), 2.718f);
  EXPECT_DOUBLE_EQ(ini.get_double_def("section", "nonexistent", 1.618), 1.618);
}

TEST_F(IniTest, ErrorHandling)
{
  inip::ini ini;

  EXPECT_THROW(ini.get("nonexistent_section", "key"), inip::err::Errors);
  EXPECT_THROW(ini.get_bool("nonexistent_section", "key"), inip::err::Errors);

  ini.set("section", "not_a_number", "abc");
  EXPECT_THROW(ini.get_int("section", "not_a_number"), inip::err::Errors);
  EXPECT_THROW(ini.get_bool("section", "not_a_number"), inip::err::Errors);

  EXPECT_NO_THROW(ini.get_int_def("section", "not_a_number", 123));
  EXPECT_EQ(ini.get_int_def("section", "not_a_number", 123), 123);
}

TEST_F(IniTest, ComplexIniFile)
{
  std::vector<inip::err::Errors> err_list;
  std::string content =
    "; Configuration file\n"
    "# Another comment\n"
    "\n"
    "[database]\n"
    "  host     = localhost  ; database host\n"
    "  port     = 3306\n"
    "  user     = root\n"
    "  password = secret#pass ; with hash symbol\n"
    "\n"
    "[server]\n"
    "address = 127.0.0.1\n"
    "timeout = 60\n"
    "enabled = true\n"
    "ratio   = 1.618\n";

  createTempFile(content);
  inip::ini ini(temp_file_name);

  auto result = ini.load_file(err_list);
  EXPECT_EQ(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);

  EXPECT_EQ(ini.get("database", "host"), "localhost");
  EXPECT_EQ(ini.get_int("database", "port"), 3306);
  EXPECT_EQ(ini.get("server", "address"), "127.0.0.1");
  EXPECT_TRUE(ini.get_bool("server", "enabled"));
  EXPECT_DOUBLE_EQ(ini.get_double("server", "ratio"), 1.618);
}

TEST_F(IniTest, FileOpenError)
{
  std::vector<inip::err::Errors> err_list;
  inip::ini ini;

  auto result = ini.load_file("nonexistent_file.ini", err_list);
  EXPECT_EQ(result.get_code_err(), inip::err::ErrCode::CANNOT_OPEN_FILE);
}

TEST_F(IniTest, ConstFunctions)
{
  std::vector<inip::err::Errors> err_list;
  std::string content =
    "[section]\n"
    "key = value\n"
    "number = 42\n"
    "flag = true\n";

  createTempFile(content);

  inip::ini ini(temp_file_name);
  ini.load_file(err_list);

  EXPECT_TRUE(ini.contains("section"));
  EXPECT_TRUE(ini.exist_key("section", "key"));
  EXPECT_EQ(ini.get("section", "key"), "value");
  EXPECT_EQ(ini.get_default("section", "nonexistent", "default"), "default");

  const inip::Section &section = ini.at("section");
  EXPECT_EQ(section.get_value("key"), "value");
}

TEST_F(IniTest, Reload)
{
  std::vector<inip::err::Errors> err_list;
  inip::ini ini;

  std::string content1 =
    "[first]\n"
    "key = value1\n";

  ini.load_str(content1, err_list);
  EXPECT_EQ(ini.get("first", "key"), "value1");
  EXPECT_FALSE(ini.contains("second"));

  std::string content2 =
    "[second]\n"
    "key = value2\n";

  ini.clear();
  err_list.clear();

  ini.load_str(content2, err_list);
  EXPECT_FALSE(ini.contains("first")); // should be cleared
  EXPECT_TRUE(ini.contains("second"));
  EXPECT_EQ(ini.get("second", "key"), "value2");
}

TEST_F(IniTest, TypeConversionEdgeCases)
{
  inip::ini ini;

  ini.set("test", "bool_true", "true");
  ini.set("test", "bool_false", "false");
  ini.set("test", "bool_yes", "yes");
  ini.set("test", "bool_no", "no");
  ini.set("test", "bool_1", "1");
  ini.set("test", "bool_0", "0");

  EXPECT_TRUE(ini.get_bool("test", "bool_true"));
  EXPECT_FALSE(ini.get_bool("test", "bool_false"));
  EXPECT_TRUE(ini.get_bool("test", "bool_yes"));
  EXPECT_FALSE(ini.get_bool("test", "bool_no"));
  EXPECT_TRUE(ini.get_bool("test", "bool_1"));
  EXPECT_FALSE(ini.get_bool("test", "bool_0"));

  ini.set("test", "hex", "0xFF");
  ini.set("test", "octal", "0777");
  ini.set("test", "negative", "-123");

  EXPECT_EQ(ini.get_int("test", "negative"), -123);
}
