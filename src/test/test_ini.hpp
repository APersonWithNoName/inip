#ifndef INI_TEST_HPP
#define INI_TEST_HPP

#include "inip/ini.hpp"
#include <cstdio>
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>
#include <vector>

class IniTest : public ::testing::Test
{
  protected:
  void SetUp() override
  {
    // 创建临时文件用于测试
    temp_file_name = "test_config.ini";
  }

  void TearDown() override
  {
    // 删除临时文件
    std::remove(temp_file_name.c_str());
  }

  void createTempFile(const std::string &content)
  {
    std::ofstream file(temp_file_name);
    file << content;
    file.close();
  }

  std::string temp_file_name;
};

#endif // INI_TEST_HPP
