#include "inip/Tools.hpp"

#include "config.h"
#include "version.h"

#include <sstream>

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
