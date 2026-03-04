#ifndef __INIP_INI_HPP__
#define __INIP_INI_HPP__

#include "inip/IniMgr.hpp"
#include "inip/Exception.hpp"
#include "inip/Section.hpp"

#include <string>
#include <vector>
#include <sstream>
#include <functional>

namespace inip {

class ini
{
private:
  inip::IniMgr inimgr;

public:
  ini(const std::string &file_name = "");

  err::Errors load_file(const std::string &file_name,
    std::vector<inip::err::Errors> &err_list, const bool failed_skip = false,
    const bool use_import=true, const bool import_cover_sec_if_exist=true,
    const bool allow_duplicate_sec=true, const bool cover_sec_if_exist=true,
    const bool allow_duplicate_keys=true, const bool cover_key_if_exist=true);
  err::Errors load_file(std::vector<inip::err::Errors> &err_list, const bool failed_skip = false,
    const bool use_import=true, const bool import_cover_sec_if_exist=true,
    const bool allow_duplicate_sec=true, const bool cover_sec_if_exist=true,
    const bool allow_duplicate_keys=true, const bool cover_key_if_exist=true);
  err::Errors load_str(const std::string &str,
    std::vector<inip::err::Errors> &err_list, const bool failed_skip = false,
    const bool use_import=true, const bool import_cover_sec_if_exist=true,
    const bool allow_duplicate_sec=true, const bool cover_sec_if_exist=true,
    const bool allow_duplicate_keys=true, const bool cover_key_if_exist=true);
  err::Errors load_sstr(std::stringstream &str,
    std::vector<inip::err::Errors> &err_list, const bool failed_skip = false,
    const bool use_import=true, const bool import_cover_sec_if_exist=true,
    const bool allow_duplicate_sec=true, const bool cover_sec_if_exist=true,
    const bool allow_duplicate_keys=true, const bool cover_key_if_exist=true);

  bool contains(const std::string &secname) const;
  bool exist_key(const std::string &secname, const std::string &key) const;
  bool exist_key(const std::string &key) const;

  void set(const std::string &secname);
  void set(const std::string &key, const std::string &value);
  void set(const std::string &secname, const std::string &key, const std::string &value);

  Section get_section(const std::string &secname);
  
  std::string get(const std::string &key) const;
  std::string get(const std::string &secname, const std::string &key) const;
  std::string get_default(const std::string &key, const std::string &def) const;
  std::string get_default(const std::string &secname, const std::string &key, const std::string &def) const;
  
  template<typename T>
  auto set_value(const std::string &secname, const std::string &key, const T value, const std::function<std::string(const T def_value)> fn) -> void
  {
    this->set(secname, key, fn(value));
  }

  template<typename T>
  auto get_value(const std::string &secname, const std::string &key, const std::function<T(const std::string raw_str)> fn) -> T const
  {
    try {
      return fn(this->get(secname, key));
    }
    catch (const inip::err::Errors &e) {
      throw e;
    }
  }

  template<typename T>
  auto get_value_def(const std::string &secname, const std::string &key, const std::function<T(const std::string raw_str)> fn, const T def) -> T const
  {
    try {
      std::string str_value = this->get(secname, key);
      return fn(str_value);
    }
    catch (...) {
      return def;
    }
  }

  // 获取节名
  template <typename T = std::vector<std::string>>
  auto section_names(const bool skip_empty = false) -> T const
  {
    T ret;
    for (const auto &pair : this->inimgr.data) {
      if (skip_empty && pair.second.empty()) {
        continue;
      }
      ret.push_back(typename T::value_type(pair.first));
    }
    return ret;
  }

  // 获取节引用
  template <typename T = std::vector<std::reference_wrapper<const ::inip::Section>>>
  auto sections(const bool skip_empty = false) -> T
  {
    T ret;
    for (const auto &pair : this->inimgr.data) {
      if (skip_empty && pair.second.empty()) {
        continue;
      }
      else {
          ret.push_back(std::cref(pair.second));
      }
    }
    return ret;
  }

  // 获取节拷贝
  template <typename T = std::vector<Section>>
  auto sections_copy(const bool skip_empty = false) -> T const
  {
    T ret;
    for (const auto &pair : this->inimgr.data) {
      if (skip_empty && pair.second.empty()) {
        ret.push_back(pair.second);
      }
    }
    return ret;
  }

  std::string to_string() const;
  
  void clear(const bool clear_name = false);
  void write();
  void write(const std::string &file_name);

  ini& operator=(const ini &ini_obj);

  inip::Section& operator[] (const std::string &sec);
  inip::Section& at(const std::string &sec);
  const inip::Section& at(const std::string &sec) const;
  std::size_t size() const;
  std::size_t max_size() const;

  void set_bool(const std::string &secname, const std::string &key, const bool value);
  bool get_bool(const std::string &secname, const std::string &key);
  bool get_bool_def(const std::string &secname, const std::string &key, const bool def);

  void set_int(const std::string &secname, const std::string &key, const int value);
  int get_int(const std::string &secname, const std::string &key);
  int get_int_def(const std::string &secname, const std::string &key, const int def);

  void set_uint(const std::string &secname, const std::string &key, const unsigned int value);
  unsigned int get_uint(const std::string &secname, const std::string &key);
  unsigned int get_uint_def(const std::string &secname, const std::string &key, const unsigned int def);

  void set_long(const std::string &secname, const std::string &key, const long value);
  long get_long(const std::string &secname, const std::string &key);
  long get_long_def(const std::string &secname, const std::string &key, const long def);

  void set_double(const std::string &secname, const std::string &key, const double value);
  double get_double(const std::string &secname, const std::string &key);
  double get_double_def(const std::string &secname, const std::string &key, const double def);

  void set_float(const std::string &secname, const std::string &key, const float value);
  float get_float(const std::string &secname, const std::string &key);
  float get_float_def(const std::string &secname, const std::string &key, const float def);
};

}


#endif
