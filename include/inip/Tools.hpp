#ifndef __INIP_TOOLS_HPP__
#define __INIP_TOOLS_HPP__

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <unordered_set>

namespace inip {
namespace Tools {

std::string get_build_info();

std::vector<std::string> split(const std::string &str, const char delim);

std::string trim(const std::string &istr);

std::pair<std::string::size_type, std::optional<char>> find_first_non_space(const std::string &str);


template<typename T1, typename T2>
void cover_map(std::map<T1, T2> &in1, const std::map<T1, T2> &in2)
{
  for (const auto& pair: in2) {
    in1[pair.first] = pair.second;
  }
}

template<typename T>
void append_sec_value(std::vector<T> vec, const T value)
{
  static std::unordered_set<T> seen(vec.begin(), vec.end());

  if (seen.find(value) == seen.end()) {
    vec.push_back(value);
    seen.insert(value);
  }
}

template<typename T>
void append_sec(std::vector<T> vec1, const std::vector<T> vec2)
{
  for (const auto& var: vec2) {
    inip::Tools::append_sec_value(vec1, var);
  }
}

}
}


#endif
