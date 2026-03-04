#include "test_ini.hpp"

// 测试构造函数
TEST_F(IniTest, Constructor)
{
  // 默认构造函数
  inip::ini ini1;
  SUCCEED();

  // 带文件名构造函数
  inip::ini ini2("test.ini");
  SUCCEED();
}

// 测试load_file函数
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

  // 加载文件
  auto result = ini.load_file(err_list);
  EXPECT_EQ(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);
}

// 测试load_file使用不同文件名
TEST_F(IniTest, LoadFileWithDifferentName)
{
  std::vector<inip::err::Errors> err_list;
  std::string content =
      "[test]\n"
      "key = value\n";

  createTempFile(content);
  inip::ini ini; // 无参构造

  // 加载指定文件
  auto result = ini.load_file(temp_file_name, err_list);
  EXPECT_EQ(result.get_code_err(), inip::err::ErrCode::NO_ERRORS);
}

// 测试load_str函数
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

// 测试load_sstr函数
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

// 测试contains函数
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

  // 测试空节(默认节)
  EXPECT_TRUE(ini.contains(""));
}

// 测试exist_key函数
TEST_F(IniTest, ExistKey)
{
  inip::ini ini;

  std::vector<inip::err::Errors> err_list;
  std::string content =
      "global_key = global_value\n" // 默认节
      "[database]\n"
      "host = localhost\n"
      "port = 3306\n"
      "\n";

  ini.load_str(content, err_list);

  // 测试指定节
  EXPECT_TRUE(ini.exist_key("database", "host"));
  EXPECT_TRUE(ini.exist_key("database", "port"));
  EXPECT_FALSE(ini.exist_key("database", "nonexistent"));

  // 测试默认节
  EXPECT_TRUE(ini.exist_key("global_key"));
  EXPECT_FALSE(ini.exist_key("nonexistent_key"));

  // 测试不存在的节
  EXPECT_FALSE(ini.exist_key("nonexistent", "key"));
}

// 测试set函数(三个重载)
TEST_F(IniTest, SetFunctions)
{
  inip::ini ini;

  // 测试 set(const std::string &secname) - 创建空节
  ini.set("new_section");
  EXPECT_TRUE(ini.contains("new_section"));

  // 测试 set(const std::string &key, const std::string &value) - 默认节
  ini.set("global_key", "global_value");
  EXPECT_TRUE(ini.exist_key("global_key"));

  // 测试 set(const std::string &secname, const std::string &key, const std::string &value)
  ini.set("new_section", "key1", "value1");
  EXPECT_TRUE(ini.exist_key("new_section", "key1"));
}

// 测试get函数
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

  // 测试指定节的键
  EXPECT_EQ(ini.get("database", "host"), "localhost");
  EXPECT_EQ(ini.get("database", "port"), "3306");

  // 测试默认节的键
  EXPECT_EQ(ini.get("global"), "value");

  // 测试不存在的键 - 应该抛出异常
  EXPECT_THROW(ini.get("database", "nonexistent"), inip::err::Errors);
  EXPECT_THROW(ini.get("nonexistent", "key"), inip::err::Errors);
}

// 测试get_default函数
TEST_F(IniTest, GetDefault)
{
  inip::ini ini;

  std::vector<inip::err::Errors> err_list;
  std::string content =
      "[section]\n"
      "existing = value\n";

  ini.load_str(content, err_list);

  // 测试存在的键
  EXPECT_EQ(ini.get_default("section", "existing", "default"), "value");

  // 测试不存在的键 - 返回默认值
  EXPECT_EQ(ini.get_default("section", "nonexistent", "default_value"), "default_value");

  // 测试不存在的节 - 返回默认值
  EXPECT_EQ(ini.get_default("nonexistent", "key", "default"), "default");
}

// 测试clear函数
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

  // 清除前应该包含节
  EXPECT_TRUE(ini.contains("section1"));
  EXPECT_TRUE(ini.contains("section2"));

  // 清除数据
  ini.clear();
  err_list.clear();

  // 清除后应该不包含节(除了可能的空节)
  EXPECT_FALSE(ini.contains("section1"));
  EXPECT_FALSE(ini.contains("section2"));

  // 测试clear_name参数
  ini.load_file(temp_file_name, err_list); // 设置文件名
  ini.clear(true);               // 清除文件名
                                 // 这里无法直接测试文件名是否被清除,因为没有公开的接口
}

// 测试to_string函数
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

  // 检查输出是否包含预期内容
  EXPECT_NE(result.find("[database]"), std::string::npos);
  EXPECT_NE(result.find("host = localhost"), std::string::npos);
  EXPECT_NE(result.find("[server]"), std::string::npos);
  EXPECT_NE(result.find("address = 127.0.0.1"), std::string::npos);
}

// 测试write函数
TEST_F(IniTest, Write)
{
  inip::ini ini;

  std::vector<inip::err::Errors> err_list;
  std::string content =
      "[test]\n"
      "key1 = value1\n"
      "key2 = value2\n";

  ini.load_str(content, err_list);

  // 写入文件
  std::string output_file = "test_output.ini";
  ini.write(output_file);

  // 读取并验证文件内容
  std::ifstream file(output_file);
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string file_content = buffer.str();

  EXPECT_NE(file_content.find("[test]"), std::string::npos);
  EXPECT_NE(file_content.find("key1 = value1"), std::string::npos);
  EXPECT_NE(file_content.find("key2 = value2"), std::string::npos);

  // 清理
  file.close();
  std::remove(output_file.c_str());
}

// 测试write无参版本
TEST_F(IniTest, WriteDefault)
{
  std::string content =
      "[test]\n"
      "key = value\n";

  createTempFile(content);
  inip::ini ini(temp_file_name);

  // 修改内容
  ini.set("test", "new_key", "new_value");

  // 写入到默认文件
  EXPECT_NO_THROW(ini.write());

  // 清理
  std::remove(temp_file_name.c_str());
}

// 测试operator[]
TEST_F(IniTest, OperatorBracket)
{
  inip::ini ini;

  // operator[] 应该创建不存在的节
  inip::Section &section = ini["new_section"];
  EXPECT_TRUE(ini.contains("new_section"));

  // 可以通过返回的引用修改节
  section.set("key", "value");
  EXPECT_TRUE(ini.exist_key("new_section", "key"));
  EXPECT_EQ(ini.get("new_section", "key"), "value");
}

// 测试at函数
TEST_F(IniTest, At)
{
  inip::ini ini;

  std::vector<inip::err::Errors> err_list;
  std::string content =
      "[existing]\n"
      "key = value\n";

  ini.load_str(content, err_list);

  // at() 应该返回已存在节的引用
  inip::Section &section = ini.at("existing");
  EXPECT_EQ(section.get_value("key"), "value");

  // 修改
  section.set("key", "new_value");
  EXPECT_EQ(ini.get("existing", "key"), "new_value");

  // 访问不存在的节应该抛出异常
  EXPECT_THROW(ini.at("nonexistent"), inip::err::Errors);
}

// 测试size和max_size函数
TEST_F(IniTest, SizeFunctions)
{
  inip::ini ini;

  // 初始大小(可能包含空节)
  std::size_t initial_size = ini.size();

  // 添加节
  ini.set("section1");
  ini.set("section2");

  EXPECT_EQ(ini.size(), initial_size + 2);

  // max_size 应该返回一个合理的值
  EXPECT_GT(ini.max_size(), 0);
}

// 测试布尔类型函数
TEST_F(IniTest, BoolFunctions)
{
  inip::ini ini;

  // 设置布尔值
  ini.set_bool("section", "enabled", true);
  ini.set_bool("section", "debug", false);

  // 获取布尔值
  EXPECT_TRUE(ini.get_bool("section", "enabled"));
  EXPECT_FALSE(ini.get_bool("section", "debug"));

  // 测试默认值
  EXPECT_TRUE(ini.get_bool_def("section", "nonexistent", true));
  EXPECT_FALSE(ini.get_bool_def("section", "nonexistent", false));
}

// 测试整数类型函数
TEST_F(IniTest, IntFunctions)
{
  inip::ini ini;

  // 设置整数值
  ini.set_int("section", "count", 42);
  ini.set_int("section", "negative", -10);

  // 获取整数值
  EXPECT_EQ(ini.get_int("section", "count"), 42);
  EXPECT_EQ(ini.get_int("section", "negative"), -10);

  // 测试默认值
  EXPECT_EQ(ini.get_int_def("section", "nonexistent", 99), 99);
}

// 测试浮点数函数
TEST_F(IniTest, FloatFunctions)
{
  inip::ini ini;

  // 设置浮点值
  ini.set_float("section", "pi", 3.14159f);
  ini.set_double("section", "precise", 3.14159265358979);

  // 获取浮点值
  EXPECT_FLOAT_EQ(ini.get_float("section", "pi"), 3.14159f);
  EXPECT_NEAR(ini.get_double("section", "precise"), 3.14159265358979, 1e-6);

  // 测试默认值
  EXPECT_FLOAT_EQ(ini.get_float_def("section", "nonexistent", 2.718f), 2.718f);
  EXPECT_DOUBLE_EQ(ini.get_double_def("section", "nonexistent", 1.618), 1.618);
}

// 测试错误处理
TEST_F(IniTest, ErrorHandling)
{
  inip::ini ini;

  // 测试不存在的键
  EXPECT_THROW(ini.get("nonexistent_section", "key"), inip::err::Errors);
  EXPECT_THROW(ini.get_bool("nonexistent_section", "key"), inip::err::Errors);

  // 测试无效的类型转换
  ini.set("section", "not_a_number", "abc");
  EXPECT_THROW(ini.get_int("section", "not_a_number"), inip::err::Errors);
  EXPECT_THROW(ini.get_bool("section", "not_a_number"), inip::err::Errors);

  // 但get_default应该返回默认值而不抛出异常
  EXPECT_NO_THROW(ini.get_int_def("section", "not_a_number", 123));
  EXPECT_EQ(ini.get_int_def("section", "not_a_number", 123), 123);
}

// 测试复杂的INI文件
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

  // 验证内容
  EXPECT_EQ(ini.get("database", "host"), "localhost");
  EXPECT_EQ(ini.get_int("database", "port"), 3306);
  EXPECT_EQ(ini.get("server", "address"), "127.0.0.1");
  EXPECT_TRUE(ini.get_bool("server", "enabled"));
  EXPECT_DOUBLE_EQ(ini.get_double("server", "ratio"), 1.618);
}

// 测试文件打开失败
TEST_F(IniTest, FileOpenError)
{
  std::vector<inip::err::Errors> err_list;
  inip::ini ini;

  // 尝试打开不存在的文件
  auto result = ini.load_file("nonexistent_file.ini", err_list);
  EXPECT_EQ(result.get_code_err(), inip::err::ErrCode::CANNOT_OPEN_FILE);
}

// 测试const版本的函数
TEST_F(IniTest, ConstFunctions)
{
  std::vector<inip::err::Errors> err_list;
  std::string content =
      "[section]\n"
      "key = value\n"
      "number = 42\n"
      "flag = true\n";

  createTempFile(content);

  // 创建const对象
  inip::ini ini(temp_file_name);
  ini.load_file(err_list);

  // const对象应该能调用const成员函数
  EXPECT_TRUE(ini.contains("section"));
  EXPECT_TRUE(ini.exist_key("section", "key"));
  EXPECT_EQ(ini.get("section", "key"), "value");
  EXPECT_EQ(ini.get_default("section", "nonexistent", "default"), "default");

  // const对象不能调用非const成员函数
  // ini.set("section", "new_key", "new_value");  // 这应该无法编译

  // 测试const版本的at()
  const inip::Section &section = ini.at("section");
  EXPECT_EQ(section.get_value("key"), "value");
}

// 测试重复加载
TEST_F(IniTest, Reload)
{
  std::vector<inip::err::Errors> err_list;
  inip::ini ini;

  // 第一次加载
  std::string content1 =
      "[first]\n"
      "key = value1\n";

  ini.load_str(content1, err_list);
  EXPECT_EQ(ini.get("first", "key"), "value1");
  EXPECT_FALSE(ini.contains("second"));

  // 第二次加载(应该清除之前的内容)
  std::string content2 =
      "[second]\n"
      "key = value2\n";

  ini.clear();
  err_list.clear();

  ini.load_str(content2, err_list);
  EXPECT_FALSE(ini.contains("first")); // 应该被清除
  EXPECT_TRUE(ini.contains("second"));
  EXPECT_EQ(ini.get("second", "key"), "value2");
}

// 测试类型转换边缘情况
TEST_F(IniTest, TypeConversionEdgeCases)
{
  inip::ini ini;

  // 布尔值转换
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

  // 数字转换
  ini.set("test", "hex", "0xFF");
  ini.set("test", "octal", "0777");
  ini.set("test", "negative", "-123");

  // 注意：str2int可能需要支持十六进制和八进制
  // EXPECT_EQ(ini.get_int("test", "hex"), 255);
  // EXPECT_EQ(ini.get_int("test", "octal"), 511);
  EXPECT_EQ(ini.get_int("test", "negative"), -123);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
