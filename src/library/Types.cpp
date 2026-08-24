#include "inip/Types.hpp"
#include "inip/Exception.hpp"

#include <cctype>
#include <cstdlib>
#include <limits>
#include <cerrno>
#include <string>

auto inip::Types::bool2str(const bool b) -> std::string
{
  if (b)
    return "true";
  return "false";
}

auto inip::Types::str2bool(const std::string &s) -> bool
{
  std::string lower_str;
  for (unsigned char c : s) {
    lower_str.push_back(std::tolower(c));
  }
  if (lower_str == "true" || lower_str == "1" || lower_str == "yes" || lower_str == "on")
    return true;
  if (lower_str == "false" || lower_str == "0" || lower_str == "no" || lower_str == "off")
    return false;
  throw inip::err::Errors(inip::err::ErrCode::INVALID_VALUE);
}

auto inip::Types::str2uint(const std::string &s) -> unsigned int
{
  if (s.empty()) {
    throw inip::err::Errors(inip::err::ErrCode::INVALID_VALUE);
  }

  char* endptr = nullptr;
  errno = 0;

  unsigned long result = strtoul(s.c_str(), &endptr, 10);

  // check if convert the full string
  if (endptr == s.c_str() || *endptr != '\0') {
    throw inip::err::Errors(inip::err::ErrCode::INVALID_VALUE);
  }

  // check overflow
  if (errno == ERANGE || result > std::numeric_limits<unsigned int>::max()) {
    throw inip::err::Errors(inip::err::ErrCode::INVALID_VALUE);
  }

  if (s.find('-') != std::string::npos) {
    throw inip::err::Errors(inip::err::ErrCode::INVALID_VALUE);
  }

  return static_cast<unsigned int>(result);
}

auto inip::Types::str2int(const std::string &s) -> int
{
  try {
    return std::stoi(s);
  }
  catch (...) {
    throw inip::err::Errors(inip::err::ErrCode::INVALID_VALUE);
  }
}

auto inip::Types::str2long(const std::string &s) -> long
{
  try {
    if (s.empty()) {
      throw inip::err::Errors(inip::err::ErrCode::INVALID_VALUE);
    }

    char* endptr = nullptr;
    errno = 0;

    // use strtol
    long value = strtol(s.c_str(), &endptr, 10);

    if (endptr == s.c_str() || *endptr != '\0') {
      throw inip::err::Errors(inip::err::ErrCode::INVALID_VALUE);
    }

    if (errno == ERANGE) {
      throw inip::err::Errors(inip::err::ErrCode::INVALID_VALUE);
    }

    return value;
  }
  catch (...) {
    throw inip::err::Errors(inip::err::ErrCode::INVALID_VALUE);
  }
}

auto inip::Types::str2ulong(const std::string &s) -> unsigned long
{
  try {
    if (s.empty()) {
      throw inip::err::Errors(inip::err::ErrCode::INVALID_VALUE);
    }

    if (s.find('-') != std::string::npos) {
      throw inip::err::Errors(inip::err::ErrCode::INVALID_VALUE);
    }

    char* endptr = nullptr;
    errno = 0;
    unsigned long value = std::strtoul(s.c_str(), &endptr, 10);

    if (endptr == s.c_str() || *endptr != '\0') {
      throw inip::err::Errors(inip::err::ErrCode::INVALID_VALUE);
    }

    if (errno == ERANGE) {
      throw inip::err::Errors(inip::err::ErrCode::INVALID_VALUE);
    }

    return value;
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
