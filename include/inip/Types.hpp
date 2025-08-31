#ifndef __INIP_TYPES_HPP__
#define __INIP_TYPES_HPP__

#include <string>
#include <sstream>

namespace inip {

namespace Types {

auto bool2str(const bool b) -> std::string;
auto str2bool(const std::string &s) -> bool;

template<typename T>
auto __num2string(const T x) -> std::string
{
  return std::to_string(x);
}

auto str2int(const std::string &s) -> int;
auto str2long(const std::string &s) -> long;
auto str2ulong(const std::string &s) -> unsigned long;
auto str2longlong(const std::string &s) -> long long;
auto str2ulonglong(const std::string &s) -> unsigned long long;
auto str2float(const std::string &s) -> float;
auto str2double(const std::string &s) -> double;

template<typename T>
auto __num2str__ss(const T x) -> std::string
{
  std::ostringstream oss;
  oss << x;
  return oss.str();
}

template<typename T>
auto __str2num__ss(const std::string &s) -> T
{
  std::istringstream iss(s);
  T x;
  iss >> x;
  return x;
}

}

}


#endif
