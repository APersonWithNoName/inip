#ifndef INI_TEST_HPP
#define INI_TEST_HPP

#include <cstdio>
#include <fstream>
#include <gtest/gtest.h>

class IniTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    temp_file_name = "test_config.ini";
  }

  void TearDown() override
  {
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
