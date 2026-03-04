#include "inip/ini.hpp"
#include "inip/Types.hpp"
#include "inip/IniMgr.hpp"
#include "inip/Section.hpp"
#include "inip/Exception.hpp"

#include <fstream>
#include <sstream>

inip::ini::ini(const std::string &file_name) : inimgr(file_name) {}

inip::err::Errors inip::ini::load_file(const std::string &file_name,
  std::vector<inip::err::Errors> &err_list, const bool failed_skip,
  const bool use_import, const bool import_cover_sec_if_exist,
  const bool allow_duplicate_sec, const bool cover_sec_if_exist,
  const bool allow_duplicate_keys, const bool cover_key_if_exist)
{
  this->clear();
  return this->inimgr.parse_file(file_name, err_list, failed_skip, use_import, import_cover_sec_if_exist, allow_duplicate_sec, cover_sec_if_exist, allow_duplicate_keys, cover_key_if_exist);
}

inip::err::Errors inip::ini::load_file(std::vector<inip::err::Errors> &err_list, const bool failed_skip,
  const bool use_import, const bool import_cover_sec_if_exist,
  const bool allow_duplicate_sec, const bool cover_sec_if_exist,
  const bool allow_duplicate_keys, const bool cover_key_if_exist)
{
  this->clear();
  return this->load_file(this->inimgr.file_name, err_list, failed_skip, use_import, import_cover_sec_if_exist, allow_duplicate_sec, cover_sec_if_exist, allow_duplicate_keys, cover_key_if_exist);
}

inip::err::Errors inip::ini::load_str(const std::string &str,
  std::vector<inip::err::Errors> &err_list, const bool failed_skip,
  const bool use_import, const bool import_cover_sec_if_exist,
  const bool allow_duplicate_sec, const bool cover_sec_if_exist,
  const bool allow_duplicate_keys, const bool cover_key_if_exist)
{
  std::istringstream iss(str);
  return this->inimgr.parse_str(iss, err_list, failed_skip, use_import, import_cover_sec_if_exist, allow_duplicate_sec, cover_sec_if_exist, allow_duplicate_keys, cover_key_if_exist);
}

inip::err::Errors inip::ini::load_sstr(std::stringstream &str,
  std::vector<inip::err::Errors> &err_list, const bool failed_skip,
  const bool use_import, const bool import_cover_sec_if_exist,
  const bool allow_duplicate_sec, const bool cover_sec_if_exist,
  const bool allow_duplicate_keys, const bool cover_key_if_exist)
{
  this->clear();
  std::istringstream iss(str.str());
  return this->inimgr.parse_str(iss, err_list, failed_skip, use_import, import_cover_sec_if_exist, allow_duplicate_sec, cover_sec_if_exist, allow_duplicate_keys, cover_key_if_exist);
}

bool inip::ini::contains(const std::string &key) const
{
  return this->inimgr.data.find(key) != this->inimgr.data.end();
}

bool inip::ini::exist_key(const std::string &secname, const std::string &key) const
{
  if (this->contains(secname)) {
    if (this->inimgr.data.at(secname).contains(key)) return true;
    return false;
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
  for (auto i = this->inimgr.data.begin(); i != this->inimgr.data.end(); i++) {
    ret += i->second.to_string() + "\n";
  }
  return ret;
}

void inip::ini::write(const std::string &file_name)
{
  std::fstream fobj;
  fobj.open(file_name, std::ios::out | std::ios::trunc);
  fobj << this->to_string() << std::endl;
  fobj.close();
}

void inip::ini::write()
{
  this->write(this->inimgr.file_name);
}

inip::Section& inip::ini::operator[](const std::string &sec)
{
  return this->inimgr.data[sec];
}

inip::ini& inip::ini::operator=(const ini &ini_obj)
{
  if (this != &ini_obj) {
    this->inimgr.data = ini_obj.inimgr.data;
    this->inimgr.file_name = ini_obj.inimgr.file_name;
    this->inimgr.sec_list = ini_obj.inimgr.sec_list;
  }
  return *this;
}

inip::Section& inip::ini::at(const std::string &sec)
{
  auto it = this->inimgr.data.find(sec);
  if (it == this->inimgr.data.end())
    throw inip::err::Errors(inip::err::ErrCode::NO_SUCH_SECTION);
  return this->inimgr.data.at(sec);
}

const inip::Section& inip::ini::at(const std::string &sec) const
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


void inip::ini::set_bool(const std::string &secname, const std::string &key, const bool value)
{
  this->set_value<bool>(secname, key, value, inip::Types::bool2str);
}
bool inip::ini::get_bool(const std::string &secname, const std::string &key)
{
  try {
    return this->get_value<bool>(secname, key, inip::Types::str2bool);
  }
  catch(inip::err::Errors &e) {
    throw e;
  }
}
bool inip::ini::get_bool_def(const std::string &secname, const std::string &key, const bool def)
{
  return this->get_value_def<bool>(secname, key, inip::Types::str2bool, def);
}


void inip::ini::set_int(const std::string &secname, const std::string &key, const int value)
{
  this->set_value<int>(secname, key, value, inip::Types::__num2string<int>);
}
int inip::ini::get_int(const std::string &secname, const std::string &key)
{
  try {
    return this->get_value<int>(secname, key, inip::Types::str2int);
  }
  catch(inip::err::Errors &e) {
    throw e;
  }
}
int inip::ini::get_int_def(const std::string &secname, const std::string &key, const int def)
{
  return this->get_value_def<int>(secname, key, inip::Types::str2int, def);
}


void inip::ini::set_long(const std::string &secname, const std::string &key, const long value)
{
  this->set_value<long>(secname, key, value, inip::Types::__num2string<long>);
}
long inip::ini::get_long(const std::string &secname, const std::string &key)
{
  try {
    return this->get_value<long>(secname, key, inip::Types::str2long);
  }
  catch(inip::err::Errors &e) {
    throw e;
  }
}
long inip::ini::get_long_def(const std::string &secname, const std::string &key, const long def)
{
  return this->get_value_def<long>(secname, key, inip::Types::str2long, def);
}

void inip::ini::set_double(const std::string &secname, const std::string &key, const double value)
{
  this->set_value<double>(secname, key, value, inip::Types::__num2string<double>);
}
double inip::ini::get_double(const std::string &secname, const std::string &key)
{
  try {
    return this->get_value<double>(secname, key, inip::Types::str2double);
  }
  catch(inip::err::Errors &e) {
    throw e;
  }
}
double inip::ini::get_double_def(const std::string &secname, const std::string &key, const double def)
{
  return this->get_value_def<double>(secname, key, inip::Types::str2double, def);
}


void inip::ini::set_float(const std::string &secname, const std::string &key, const float value)
{
  this->set_value<float>(secname, key, value, inip::Types::__num2string<float>);
}
float inip::ini::get_float(const std::string &secname, const std::string &key)
{
  try {
    return this->get_value<float>(secname, key, inip::Types::str2float);
  }
  catch(inip::err::Errors &e) {
    throw e;
  }
}
float inip::ini::get_float_def(const std::string &secname, const std::string &key, const float def)
{
  return this->get_value_def<float>(secname, key, inip::Types::str2float, def);
}


