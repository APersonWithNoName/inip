/**
 * @file Types.hpp
 * @brief inip Basic types and convertor
 * @author APersonWithNoName
 * @version 0.1.0
 * @date 2026-08-20
 */
#ifndef __INIP_TYPES_HPP__
#define __INIP_TYPES_HPP__

#include <sstream>
#include <string>

namespace inip
{

namespace Types
{

typedef unsigned long long LineNum;

/// @name Convertor from string to others
/// @{
auto bool2str(const bool b) -> std::string;
auto str2bool(const std::string &s) -> bool;
auto str2uint(const std::string &s) -> unsigned int;
auto str2int(const std::string &s) -> int;
auto str2long(const std::string &s) -> long;
auto str2ulong(const std::string &s) -> unsigned long;
auto str2longlong(const std::string &s) -> long long;
auto str2ulonglong(const std::string &s) -> unsigned long long;
auto str2float(const std::string &s) -> float;
auto str2double(const std::string &s) -> double;
template <typename T>
auto __str2num__ss(const std::string &s) -> T
{
  std::istringstream iss(s);
  T x;
  iss >> x;
  return x;
}
/// @}

/// @name Convertor from others to string
/// @{
template <typename T>
auto __num2string(const T x) -> std::string
{
  return std::to_string(x);
}

template <typename T>
auto __num2str__ss(const T x) -> std::string
{
  std::ostringstream oss;
  oss << x;
  return oss.str();
}
/// @}


} // namespace Types

} // namespace inip


#endif
