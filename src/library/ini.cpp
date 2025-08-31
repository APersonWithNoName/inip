#include "inip/ini.hpp"
#include "inip/Types.hpp"
#include "inip/IniMgr.hpp"
#include "inip/Section.hpp"
#include "inip/Exception.hpp"

#include "config.h"

#include <fstream>

inip::ini::ini(const std::string &file_name) : inimgr(file_name) {}

inip::err::Errors inip::ini::load_file()
{
  return this->inimgr.parse_file();
}

bool inip::ini::is_section_exists(const std::string &key)
{
  if (this->inimgr.data.find(key) == this->inimgr.data.end()) {
    return false;
  }
  return true;
}

void inip::ini::add(const std::string &secname)
{
#ifdef INIP_DISABLE_APPEND_SECTION
  if (this->is_section_exists(secname)) {
    throw inip::err::Errors(inip::err::ErrCode::SEC_EXISTS);
  }
#else
  if (!this->is_section_exists(secname)) {
    this->inimgr.data[secname] = inip::Section(secname, {});
  }
#endif
}

void inip::ini::add(const std::string &secname, const std::string &key, const std::string &value)
{
  if (!this->is_section_exists(secname)) {
    this->inimgr.data[secname] = inip::Section(secname, {});
  }
#ifdef INIP_DISABLE_RRADD_NODE_KEY
  if (this->inimgr.data[secname].is_key_exist(key)) {
    throw inip::err::Errors(inip::err::ErrCode::KEY_EXISTS);
  }
#else
  #ifdef INIP_COVER_KEY_IF_EXIST
    this->inimgr.data[secname].set(key, value);
  #else
    if (!this->inimgr.data[secname].is_key_exist(key)) {
      this->inimgr.data[secname].set(key, value);
    }
  #endif
#endif
}

void inip::ini::add(const std::string &key, const std::string &value)
{
  try {
    this->add("", key, value);
  }
  catch (const inip::err::Errors &e) {
    throw e;
  }
}

void inip::ini::set(const std::string &secname)
{
  this->inimgr.data[secname] = inip::Section(secname, {});
}

void inip::ini::set(const std::string &key, const std::string &value)
{
  this->add("", key, value);
}

void inip::ini::set(const std::string &secname, const std::string &key, const std::string &value)
{
  this->inimgr.data[secname].set(key, value);
}

std::string inip::ini::get(const std::string &secname, const std::string &key)
{
  if (!this->is_section_exists(secname))
    throw inip::err::Errors(inip::err::ErrCode::NO_SUCH_KEY);
  try {
    return this->inimgr.data[secname].get_value(key);
  }
  catch (const inip::err::Errors &e) {
    throw e;
  }
}

std::string inip::ini::get(const std::string &key)
{
  try {
    return this->get("", key);
  }
  catch (const inip::err::Errors &e) {
    throw e;
  }
}

std::string inip::ini::get_default(const std::string &secname, const std::string &key, const std::string &def)
{
  try {
    return this->get(secname, key);
  }
  catch (...) {
    return def;
  }
}

std::string inip::ini::get_default(const std::string &key, const std::string &def)
{
  return this->get_default("", key, def);
}

std::vector<std::string> inip::ini::get_all_sec()
{
  std::vector<std::string> ret;
  for (auto i = this->inimgr.data.begin(); i != this->inimgr.data.end(); i++) {
    ret.push_back(i->first);
  }
  return ret;
}

void inip::ini::clear()
{
  this->inimgr.data.clear();
#ifdef INIP_INI_CLEAR_NAME
  this->inimgr.file_name.clear();
#endif
}

std::string inip::ini::to_string()
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
  fobj.open(file_name, std::ios::in | std::ios::out);
  fobj << this->to_string() << std::endl;
  fobj.close();
  fobj.clear();
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


void inip::ini::set_ulong(const std::string &secname, const std::string &key, const unsigned long value)
{
  this->set_value<unsigned long>(secname, key, value, inip::Types::__num2string<unsigned long>);
}
unsigned long inip::ini::get_ulong(const std::string &secname, const std::string &key)
{
  try {
    return this->get_value<unsigned long>(secname, key, inip::Types::str2ulong);
  }
  catch(inip::err::Errors &e) {
    throw e;
  }
}
unsigned long inip::ini::get_ulong_def(const std::string &secname, const std::string &key, const unsigned long def)
{
  return this->get_value_def<unsigned long>(secname, key, inip::Types::str2ulong, def);
}


void inip::ini::set_longlong(const std::string &secname, const std::string &key, const long long value)
{
  this->set_value<long long>(secname, key, value, inip::Types::__num2string<long long>);
}
long long inip::ini::get_longlong(const std::string &secname, const std::string &key)
{
  try {
    return this->get_value<long long>(secname, key, inip::Types::str2longlong);
  }
  catch(inip::err::Errors &e) {
    throw e;
  }
}
long long inip::ini::get_longlong_def(const std::string &secname, const std::string &key, const long long def)
{
  return this->get_value_def<long long>(secname, key, inip::Types::str2longlong, def);
}


void inip::ini::set_ulonglong(const std::string &secname, const std::string &key, const unsigned long long value)
{
  this->set_value<unsigned long long>(secname, key, value, inip::Types::__num2string<unsigned long long>);
}
unsigned long long inip::ini::get_ulonglong(const std::string &secname, const std::string &key)
{
  try {
    return this->get_value<unsigned long long>(secname, key, inip::Types::str2ulonglong);
  }
  catch(inip::err::Errors &e) {
    throw e;
  }
}
unsigned long long inip::ini::get_ulonglong_def(const std::string &secname, const std::string &key, const unsigned long long def)
{
  return this->get_value_def<unsigned long long>(secname, key, inip::Types::str2ulonglong, def);
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


