#include "inip/ini.hpp"
#include "inip/Exception.hpp"
#include "inip/IniMgr.hpp"
#include "inip/Section.hpp"
#include "inip/Types.hpp"

#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


inip::ini::ini(const std::string &file_name) : inimgr(file_name)
{}

inip::err::Errors inip::ini::load_file(const std::string &file_name,
                                       std::vector<inip::err::Errors> &err_list,
                                       const bool failed_skip,
                                       const bool use_import,
                                       const bool import_cover_sec_if_exist,
                                       const bool allow_duplicate_sec,
                                       const bool cover_sec_if_exist,
                                       const bool allow_duplicate_keys,
                                       const bool cover_key_if_exist)
{
  return this->inimgr.parse_file(file_name,
                                 err_list,
                                 failed_skip,
                                 use_import,
                                 import_cover_sec_if_exist,
                                 allow_duplicate_sec,
                                 cover_sec_if_exist,
                                 allow_duplicate_keys,
                                 cover_key_if_exist);
}

inip::err::Errors inip::ini::load_file(std::vector<inip::err::Errors> &err_list,
                                       const bool failed_skip,
                                       const bool use_import,
                                       const bool import_cover_sec_if_exist,
                                       const bool allow_duplicate_sec,
                                       const bool cover_sec_if_exist,
                                       const bool allow_duplicate_keys,
                                       const bool cover_key_if_exist)
{
  return this->load_file(this->inimgr.file_name,
                         err_list,
                         failed_skip,
                         use_import,
                         import_cover_sec_if_exist,
                         allow_duplicate_sec,
                         cover_sec_if_exist,
                         allow_duplicate_keys,
                         cover_key_if_exist);
}

inip::err::Errors inip::ini::load_str(const std::string &str,
                                      std::vector<inip::err::Errors> &err_list,
                                      const bool failed_skip,
                                      const bool use_import,
                                      const bool import_cover_sec_if_exist,
                                      const bool allow_duplicate_sec,
                                      const bool cover_sec_if_exist,
                                      const bool allow_duplicate_keys,
                                      const bool cover_key_if_exist)
{
  std::istringstream iss(str);
  return this->inimgr.parse_str(iss,
                                err_list,
                                failed_skip,
                                use_import,
                                import_cover_sec_if_exist,
                                allow_duplicate_sec,
                                cover_sec_if_exist,
                                allow_duplicate_keys,
                                cover_key_if_exist);
}

inip::err::Errors inip::ini::load_sstr(std::stringstream &str,
                                       std::vector<inip::err::Errors> &err_list,
                                       const bool failed_skip,
                                       const bool use_import,
                                       const bool import_cover_sec_if_exist,
                                       const bool allow_duplicate_sec,
                                       const bool cover_sec_if_exist,
                                       const bool allow_duplicate_keys,
                                       const bool cover_key_if_exist)
{
  std::istringstream iss(str.str());
  return this->inimgr.parse_str(iss,
                                err_list,
                                failed_skip,
                                use_import,
                                import_cover_sec_if_exist,
                                allow_duplicate_sec,
                                cover_sec_if_exist,
                                allow_duplicate_keys,
                                cover_key_if_exist);
}

bool inip::ini::contains(const std::string &secname) const
{
  return this->inimgr.data.find(secname) != this->inimgr.data.end();
}

bool inip::ini::exist_key(const std::string &secname, const std::string &key) const
{
  if (this->contains(secname)) {
    return this->inimgr.data.at(secname).contains(key);
  }
  return false;
}

bool inip::ini::exist_key(const std::string &key) const
{
  return this->exist_key("", key);
}

void inip::ini::set(const std::string &secname)
{
  this->inimgr.data[secname] = inip::Section(secname, {});
}

void inip::ini::set(const std::string &key, const std::string &value)
{
  this->set("", key, value);
}

void inip::ini::set(const std::string &secname, const std::string &key, const std::string &value)
{
  if (!this->contains(secname))
    this->set(secname);
  this->inimgr.data[secname].set(key, value);
}

std::string inip::ini::get(const std::string &secname, const std::string &key) const
{
  if (!this->exist_key(secname, key))
    throw inip::err::Errors(inip::err::ErrCode::NO_SUCH_KEY, 0, this->inimgr.file_name);

  return this->inimgr.data.at(secname).get_value(key);
}

std::string inip::ini::get(const std::string &key) const
{
  try {
    return this->get("", key);
  }
  catch (const inip::err::Errors &e) {
    throw e;
  }
}

std::string inip::ini::get_default(const std::string &secname, const std::string &key, const std::string &def) const
{
  try {
    return this->get(secname, key);
  }
  catch (...) {
    return def;
  }
}

inip::Section inip::ini::get_section(const std::string &secname)
{
  if (!this->contains(secname))
    throw inip::err::Errors(inip::err::ErrCode::NO_SUCH_SECTION, 0, this->inimgr.file_name);

  return this->inimgr.data.at(secname);
}

std::string inip::ini::get_default(const std::string &key, const std::string &def) const
{
  return this->get_default("", key, def);
}


void inip::ini::clear(const bool clear_name)
{
  this->inimgr.data.clear();
  if (clear_name) {
    this->inimgr.file_name.clear();
  }
}

std::string inip::ini::to_string() const
{
  std::string ret;
  for (const auto &i : inimgr.data) {
    ret += i.second.to_string() + "\n";
  }
  return ret;
}

inip::err::Errors inip::ini::write(const std::string &file_name) const
{
  auto err = inip::err::Errors();
  std::fstream fobj;

  fobj.open(file_name, std::ios::out | std::ios::trunc);
  if (!fobj.is_open()) {
    err.set_code(inip::err::ErrCode::CANNOT_OPEN_FILE).set_file(file_name);
    return err;
  }

  fobj << this->to_string();
  if (fobj.fail()) {
    err.set_code(inip::err::ErrCode::CANNOT_WRITE_FILE).set_file(file_name);
    return err;
  }

  fobj.flush();
  if (fobj.bad()) {
    err.set_code(inip::err::ErrCode::DISK_WRITE_FAILED).set_file(file_name);
    return err;
  }

  fobj.close();
  err.set_code(inip::err::ErrCode::NO_ERRORS).set_file(file_name);

  return err;
}

inip::err::Errors inip::ini::write() const
{
  return this->write(this->inimgr.file_name);
}

inip::Section &inip::ini::operator[](const std::string &sec)
{
  return this->inimgr.data[sec];
}

inip::ini &inip::ini::operator=(const ini &ini_obj)
{
  if (this != &ini_obj) {
    this->inimgr.data = ini_obj.inimgr.data;
    this->inimgr.file_name = ini_obj.inimgr.file_name;
    this->inimgr.sec_list = ini_obj.inimgr.sec_list;
  }
  return *this;
}

inip::Section &inip::ini::at(const std::string &sec)
{
  auto it = this->inimgr.data.find(sec);
  if (it == this->inimgr.data.end())
    throw inip::err::Errors(inip::err::ErrCode::NO_SUCH_SECTION);
  return this->inimgr.data.at(sec);
}

const inip::Section &inip::ini::at(const std::string &sec) const
{
  auto it = this->inimgr.data.find(sec);
  if (it == this->inimgr.data.end())
    throw inip::err::Errors(inip::err::ErrCode::NO_SUCH_SECTION);
  return this->inimgr.data.at(sec);
}

std::size_t inip::ini::size() const
{
  return this->inimgr.data.size();
}

std::size_t inip::ini::max_size() const
{
  return this->inimgr.data.max_size();
}

inip::ini::iterator inip::ini::begin(void)
{
  return inimgr.data.begin();
}

inip::ini::iterator inip::ini::end(void)
{
  return inimgr.data.end();
}

inip::ini::reverse_iterator inip::ini::rbegin(void)
{
  return inimgr.data.rbegin();
}

inip::ini::reverse_iterator inip::ini::rend(void)
{
  return inimgr.data.rend();
}

inip::ini::const_iterator inip::ini::cbegin(void) const
{
  return inimgr.data.cbegin();
}

inip::ini::const_iterator inip::ini::cend(void) const
{
  return inimgr.data.cend();
}

inip::ini::const_reverse_iterator inip::ini::crbegin(void) const
{
  return inimgr.data.crbegin();
}

inip::ini::const_reverse_iterator inip::ini::crend(void) const
{
  return inimgr.data.crend();
}

#define _INIP_GENERATE_DEFINE_FUNC(type_display, type_real, type_fn_st, type_fn_ts)                                      \
  void inip::ini::set_##type_display(const std::string &secname, const std::string &key, const type_real value)          \
  {                                                                                                                      \
    this->set_value<type_real>(secname, key, value, type_fn_ts);                                                         \
  }                                                                                                                      \
  type_real inip::ini::get_##type_display(const std::string &secname, const std::string &key)                            \
  {                                                                                                                      \
    return this->get_value<type_real>(secname, key, type_fn_st);                                                         \
  }                                                                                                                      \
  type_real inip::ini::get_##type_display##_def(const std::string &secname, const std::string &key, const type_real def) \
  {                                                                                                                      \
    return this->get_value_def<type_real>(secname, key, type_fn_st, def);                                                \
  }

_INIP_GENERATE_DEFINE_FUNC(bool, bool, inip::Types::str2bool, inip::Types::bool2str)
_INIP_GENERATE_DEFINE_FUNC(int, int, inip::Types::str2int, inip::Types::__num2string<int>)
_INIP_GENERATE_DEFINE_FUNC(uint, unsigned int, inip::Types::str2uint, inip::Types::__num2string<unsigned int>)
_INIP_GENERATE_DEFINE_FUNC(long, long, inip::Types::str2long, inip::Types::__num2string<long>)
_INIP_GENERATE_DEFINE_FUNC(double, double, inip::Types::str2double, inip::Types::__num2string<double>)
_INIP_GENERATE_DEFINE_FUNC(float, float, inip::Types::str2float, inip::Types::__num2string<float>)

#undef _INIP_GENERATE_DEFINE_FUNC
