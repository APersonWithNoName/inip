/**
 * @file Tools.hpp
 * @brief All kinds of tools
 * @author APersonWithNoName
 * @version 0.1.0
 * @date 2026-08-20
 */
#ifndef __INIP_TOOLS_HPP__
#define __INIP_TOOLS_HPP__

#include <map>
#include <string>
#include <unordered_set>
#include <vector>


namespace inip
{
namespace Tools
{

/**
 * @brief split Split string with char
 *
 * @param str Source string
 * @param delim Delimiter
 *
 * @return Strings in vector format
 */
std::vector<std::string> split(const std::string &str, const char delim);

/**
 * @brief split Split string with string
 *
 * @param str Source
 * @param delim Delimiter
 * @param skip_empty Whether skip empty strings
 *
 * @return Strings in vector format
 */
std::vector<std::string> split(const std::string &str, const std::string &delim, const bool skip_empty = true);

/**
 * @brief trim Trim the back and front whitespace chars
 */
std::string trim(const std::string &istr);

/**
 * @brief cover_map Insert in2 to in1, skip duplicate values
 *
 * @param in1 Map to be covered
 * @param in2 Map to cover
 */
template <typename T1, typename T2>
void cover_map(std::map<T1, T2> &in1, const std::map<T1, T2> &in2)
{
  for (const auto &pair : in2) {
    in1[pair.first] = pair.second;
  }
}

/**
 * @brief vector_equal_one Check whether the input equals one in the vector
 *
 * @param input Input value
 * @param vec Target vector
 */
template <typename T>
bool vector_equal_one(const T &input, const std::vector<T> &vec)
{
  for (const auto &i : vec) {
    if (input == i)
      return true;
  }
  return false;
}

/**
 * @brief vector_equal_all Check whether the input equals all in the vector
 *
 * @param input Input value
 * @param vec Target vector
 *
 * @details Usually used to check whether the input donot equals all in the vector
 */
template <typename T>
bool vector_equal_all(const T &input, const std::vector<T> &vec)
{
  for (const auto &i : vec) {
    if (input == i)
      return false;
  }
  return true;
}

/**
 * @brief append_sec_value Insert a value to a vector without duplicate
 *
 * @param vec Target value
 * @param value Value
 */
template <typename T>
void append_sec_value(std::vector<T> &vec, const T &value)
{
  std::unordered_set<T> seen(vec.begin(), vec.end());

  if (seen.find(value) == seen.end()) {
    vec.push_back(value);
    seen.insert(value);
  }
}

/**
 * @brief append_sec Insert vec2 to vec1, skip duplicate values
 */
template <typename T>
void append_sec(std::vector<T> &vec1, const std::vector<T> vec2)
{
  for (const auto &var : vec2) {
    inip::Tools::append_sec_value(vec1, var);
  }
}

/**
 * @brief remove_escape Remove escape chars
 */
std::string remove_escape(const std::string &s);

/**
 * @brief add_escape Add escape chars
 */
std::string add_escape(const std::string &s);

} // namespace Tools
} // namespace inip


#endif
