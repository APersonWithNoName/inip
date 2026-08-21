#include "inip/Tools.hpp"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>


std::vector<std::string> inip::Tools::split(const std::string &str, const char delim)
{
  std::vector<std::string> result;
  if (str.empty()) {
    return result;
  }

  std::stringstream ss(str);
  std::string token;

  while (std::getline(ss, token, delim)) {
    result.push_back(token);
  }

  // std::getline will not set empty string to a delimiter at the end
  // need add by hand
  if (!str.empty() && str.back() == delim) {
    result.push_back("");
  }

  return result;
}

std::vector<std::string> inip::Tools::split(const std::string &str, const std::string &delim, const bool skip_empty)
{
  std::vector<std::string> result;

  // if delimiter is empty, set every char to a element
  if (delim.empty()) {
    for (const char ch : str) {
      result.push_back(std::string(1, ch));
    }
    return result;
  }

  if (str.empty()) {
    return result;
  }

  size_t pos = 0, prev = 0;

  while ((pos = str.find(delim, prev)) != std::string::npos) {
    std::string token = str.substr(prev, pos - prev);
    if (!skip_empty || !token.empty()) {
      result.push_back(token);
    }
    prev = pos + delim.length();
  }

  std::string lastToken = str.substr(prev);
  if (!skip_empty || !lastToken.empty()) {
    result.push_back(lastToken);
  }

  return result;
}

std::string inip::Tools::trim(const std::string &istr)
{
  auto front = std::find_if_not(istr.begin(), istr.end(), [](unsigned char c) {
    return std::isspace(c);
  });
  auto back = std::find_if_not(istr.rbegin(), istr.rend(), [](unsigned char c) {
                return std::isspace(c);
              }).base();

  if (front >= back) {
    return "";
  }

  return {front, back};
}

std::string inip::Tools::add_escape(const std::string &s)
{
  std::string result;

  for (const auto i : s) {
    switch (i) {
    case ';':
    case '#':
    case '=':
    case '[':
      result.push_back('\\');
      result.push_back(i);
      break;

    default:
      result.push_back(i);
    }
  }

  return result;
}

std::string inip::Tools::remove_escape(const std::string &s)
{
  if (s.empty())
    return "";

  std::string result;

  for (std::string::size_type i = 0; i < s.size(); i++) {
    char t = s[i];

    if (t == '\\' && i + 1 < s.size()) {
      switch (s[i + 1]) {
      case ';':
        result.push_back(';');
        i++;
        break;
      case '#':
        result.push_back('#');
        i++;
        break;
      case '=':
        result.push_back('=');
        i++;
        break;
      case '[':
        result.push_back('[');
        i++;
        break;
      default:
        // invalid escape char
        result.push_back(t);
        break;
      }
    }
    else {
      result.push_back(t);
    }
  }

  return result;
}
