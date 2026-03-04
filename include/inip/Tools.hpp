#ifndef __INIP_TOOLS_HPP__
#define __INIP_TOOLS_HPP__

#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <functional>

#ifdef DEBUG__
#define DEBUG(msg) \
{std::cout << "DEBUG ["<<__FILE__<<":"<<__LINE__<<":"<<__FUNCTION__<<"] "<<msg<<std::endl;}
#else
#define DEBUG(msg)
#endif

namespace inip {
namespace Tools {

std::vector<std::string> split(const std::string &str, const char delim);

std::vector<std::string> split(const std::string &str, const std::string &delim, const bool skip_empty = true);

std::string trim(const std::string &istr);

template<typename T1, typename T2>
void cover_map(std::map<T1, T2> &in1, const std::map<T1, T2> &in2)
{
  for (const auto& pair: in2) {
    in1[pair.first] = pair.second;
  }
}

template<typename T>
bool vector_equal_one(const T input, const std::vector<T> &vec)
{
  for (const auto &i : vec) {
    if (input == i) return true;
  }
  return false;
}

template<typename T>
bool vector_equal_all(const T input, const std::vector<T> &vec)
{
  for (const auto &i : vec) {
    if (input == i) return false;
  }
  return true;
}

template<typename T>
void append_sec_value(std::vector<T> vec, const T value)
{
  std::unordered_set<T> seen(vec.begin(), vec.end());

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

struct IniPath {
  std::string file;
  std::string section;
  std::string key;
  bool valid = false;
};

IniPath parse_ini_path(const std::string &str);

struct VarExpr {
  std::string name;
  std::string def;
  bool has_def = false;
};

struct VarExprDefaultFn {
  std::string value = "";
  int abort = 0;
  std::string msg = "";
};

VarExprDefaultFn varexpr_default(const VarExpr varexpr);

VarExpr parse_varexpr(const std::string &var_str);
std::string replace_str(const std::string &str,
  const std::map<std::string, std::string> varlist,
  const std::function<VarExprDefaultFn(const VarExpr varexpr)> default_value = varexpr_default);

}
}


#endif
