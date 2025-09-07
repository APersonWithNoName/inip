#ifndef __INIP_INI_HPP__
#define __INIP_INI_HPP__

#include "inip/IniMgr.hpp"
#include "inip/Exception.hpp"
#include "inip/Section.hpp"

#include <string>
#include <vector>
#include <functional>

namespace inip {

class ini
{
private:
  inip::IniMgr inimgr;

public:
  ini(const std::string &file_name);
  err::Errors load_file();
  bool is_section_exists(const std::string &key);
  void add(const std::string &secname);
  void add(const std::string &key, const std::string &value);
  void add(const std::string &secname, const std::string &key, const std::string &value);
  void set(const std::string &secname);
  void set(const std::string &key, const std::string &value);
  void set(const std::string &secname, const std::string &key, const std::string &value);
  std::string get(const std::string &key);
  std::string get(const std::string &secname, const std::string &key);
  std::string get_default(const std::string &key, const std::string &def);
  std::string get_default(const std::string &secname, const std::string &key, const std::string &def);
  std::vector<std::string> get_all_sec();
  std::string to_string();
  void clear();
  void write();
  void write(const std::string &file_name);

  inip::Section operator[] (const std::string &sec);
  inip::Section at(const std::string &sec);
  std::size_t size();
  std::size_t max_size();


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

  void set_ulong(const std::string &secname, const std::string &key, const unsigned long value);
  unsigned long get_ulong(const std::string &secname, const std::string &key);
  unsigned long get_ulong_def(const std::string &secname, const std::string &key, const unsigned long def);

  void set_longlong(const std::string &secname, const std::string &key, const long long value);
  long long get_longlong(const std::string &secname, const std::string &key);
  long long get_longlong_def(const std::string &secname, const std::string &key, const long long def);

  void set_ulonglong(const std::string &secname, const std::string &key, const unsigned long long value);
  unsigned long long get_ulonglong(const std::string &secname, const std::string &key);
  unsigned long long get_ulonglong_def(const std::string &secname, const std::string &key, const unsigned long long def);

  void set_double(const std::string &secname, const std::string &key, const double value);
  double get_double(const std::string &secname, const std::string &key);
  double get_double_def(const std::string &secname, const std::string &key, const double def);

  void set_float(const std::string &secname, const std::string &key, const float value);
  float get_float(const std::string &secname, const std::string &key);
  float get_float_def(const std::string &secname, const std::string &key, const float def);


  template<typename T>
  auto set_value(const std::string &secname, const std::string &key, const T value, std::function<std::string(T)> fn) -> void
  {
    this->set(secname, key, fn(value));
  }
  template<typename T>
  auto get_value(const std::string &secname, const std::string &key, std::function<T(std::string)> fn) -> T
  {
    try {
      return fn(this->get(secname, key));
    }
    catch (const inip::err::Errors &e) {
      throw e;
    }
  }
  template<typename T>
  auto get_value_def(const std::string &secname, const std::string &key, std::function<T(std::string)> fn, const T def) -> T
  {
    try {
      std::string str_value = this->get(secname, key);
      return fn(str_value);
    }
    catch (...) {
      return def;
    }
  }
};

}


#endif
