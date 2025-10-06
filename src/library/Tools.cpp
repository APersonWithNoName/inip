#include "inip/Tools.hpp"

#include "config.h"
#include "version.h"

#include <cctype>
#include <iterator>
#include <optional>
#include <sstream>
#include <algorithm>
#include <vector>

std::vector<std::string> inip::Tools::split(const std::string &str, const char delim)
{
  std::vector<std::string> ret;
  std::istringstream iss(str);
  std::string token;
  while(std::getline(iss, token, delim)) {
    ret.push_back(token);
  }
  return ret;
}

std::string inip::Tools::get_build_info()
{
  std::stringstream ret;
  ret << "SYSTEM_NAME " << SYSTEM_NAME << "\n"
    << "SYSTEM_PROCESSOR " << SYSTEM_PROCESSOR << "\n"
    << "BUILD_TIMESTAMP " << BUILD_TIMESTAMP << "\n"
    << "BUILD_TYPE " << BUILD_TYPE << "\n"
    << "CXX_VERSION " << CXX_VERSION << "\n"
    << "CXX_COMPILER_ID " << CXX_COMPILER_ID << "\n"
    << "CXX_COMPILER " << CXX_COMPILER << "\n"
    << "CXX_COMPILER_VERSION " << CXX_COMPILER_VERSION << "\n";
  return ret.str();
}

std::string inip::Tools::trim(const std::string &istr)
{
  auto front = std::find_if_not(istr.begin(), istr.end(), [](unsigned char c){
    return std::isspace(c);
  });
  auto back = std::find_if_not(istr.rbegin(), istr.rend(), [](unsigned char c){
    return std::isspace(c);
  }).base();

  if (front >= back) {
    return "";
  }

  return std::string(front, back);
}

std::pair<std::string::size_type, std::optional<char>> inip::Tools::find_first_non_space(const std::string &str)
{
  auto it = std::find_if(str.begin(), str.end(), [](unsigned char c) {
    return !std::isspace(c);
  });

  if (it == str.end()) {
    return {std::string::npos, std::nullopt};
  }
  return {std::distance(str.begin(), it), *it};
}
