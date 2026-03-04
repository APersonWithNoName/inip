#include "inip/Types.hpp"
#include "inip/Exception.hpp"

#include <cctype>

auto inip::Types::bool2str(const bool b) -> std::string
{
  if (b) return "true";
  else return "false";
}

auto inip::Types::str2bool(const std::string &s) -> bool
{
  std::string lower_str;
  for (unsigned char c : s) {
    lower_str.push_back(std::tolower(c));
  }
  if (lower_str == "true" || lower_str == "1" || lower_str == "yes" || lower_str == "on")
    return true;
  else if (lower_str == "false" || lower_str == "0" || lower_str == "no" || lower_str == "off")
    return false;
  else throw inip::err::Errors(inip::err::ErrCode::INVALID_VALUE);
}

auto inip::Types::str2int(const std::string &s) -> int
{
  try{
    return std::stoi(s);
  }
  catch(...) {
    throw inip::err::Errors(inip::err::ErrCode::INVALID_VALUE);
  }
}

auto inip::Types::str2long(const std::string &s) -> long
{
  try {
    return std::stol(s);
  }
  catch (...) {
    throw inip::err::Errors(inip::err::ErrCode::INVALID_VALUE);
  }
}

auto inip::Types::str2ulong(const std::string &s) -> unsigned long
{
  try {
    return std::stoul(s);
  }
  catch (...) {
    throw inip::err::Errors(inip::err::ErrCode::INVALID_VALUE);
  }
}

auto inip::Types::str2longlong(const std::string &s) -> long long
{
  try {
  return std::stoll(s);
  }
  catch (...) {
    throw inip::err::Errors(inip::err::ErrCode::INVALID_VALUE);
  }
}

auto inip::Types::str2ulonglong(const std::string &s) -> unsigned long long
{
  try {
    return std::stoull(s);
  }
  catch (...) {
    throw inip::err::Errors(inip::err::ErrCode::INVALID_VALUE);
  }
}

auto inip::Types::str2float(const std::string &s) -> float
{
  try {
    return std::stof(s);
  }
  catch (...) {
    throw inip::err::Errors(inip::err::ErrCode::INVALID_VALUE);
  }
}

auto inip::Types::str2double(const std::string &s) -> double
{
  try {
    return std::stod(s);
  }
  catch (...) {
    throw inip::err::Errors(inip::err::ErrCode::INVALID_VALUE);
  }
}

