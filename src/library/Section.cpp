#include "inip/Section.hpp"
#include "inip/Exception.hpp"
#include "config.h"

#include <sstream>
#include <stdexcept>

inip::Section::Section() : name("") {};

inip::Section::Section(const std::string &name, const std::map<std::string, Node> &data)
{
  this->data = data;
  this->name = name;
}

bool inip::Section::is_key_exist(const std::string &key)
{
  auto it = this->data.find(key);
  if (it != this->data.end())
    return true;
  return false;
}

inip::Node* inip::Section::get_node_ptr(const std::string &key)
{
  auto it = this->data.find(key);
  if (it != this->data.end()) {
    return &(it->second);
  }
  return nullptr;
}

inip::Node inip::Section::get_node(const std::string &key)
{
  /*
  auto ptr = this->get_node_ptr(key);
  if (ptr == nullptr) {
    throw inip::err::Errors(err::ErrCode::NO_SUCH_KEY);
  }
  return *ptr;
  */
  try {
    return this->data.at(key);
  }
  catch (const std::out_of_range &e) {
    throw inip::err::Errors(err::ErrCode::NO_SUCH_KEY);
  }
}

std::string inip::Section::get_value(const std::string &key)
{
  try {
    auto node = this->get_node(key);
    return node.get_value();
  } catch (const inip::err::Errors &e) {
    throw inip::err::Errors(err::ErrCode::NO_SUCH_KEY);
  }
}

std::string inip::Section::get_value_default(const std::string &key, const std::string &def)
{
  try {
    return this->get_value(key);
  }
  catch (...) {
    return def;
  }
}

void inip::Section::add(const std::string &key, const std::string &value)
{
#ifdef INIP_DISABLE_RRADD_NODE_KEY
  if (this->is_key_exist(key)) {
    throw inip::err::Errors(inip::err::ErrCode::KEY_EXISTS);
  }
  else {
    this->data.emplace(key, value);
  }
#else
  #ifdef INIP_COVER_KEY_IF_EXIST
    this->data[key] = value;
  #else
    this->data.emplace(key, value);
  #endif
#endif
}

void inip::Section::set(const std::string &key, const std::string &value)
{
  this->data[key] = value;
}

void inip::Section::clear()
{
  this->data.clear();
#ifdef INIP_SECTION_CLEAR_NAME
  this->name.clear();
#endif
}

bool inip::Section::empty()
{
  return this->data.empty();
}

std::string inip::Section::get_name()
{
  return this->name;
}

std::string inip::Section::to_string()
{
  std::stringstream ret;

  if (this->name == "") {
    ret << "";
  }
  else {
    ret << "[" << this->name << "]" << "\n";
  }
  
  for (auto it = this->data.begin(); it != this->data.end(); it++) {
    ret << it->first << "=" << it->second.get_value() << "\n";
  }

  return ret.str();
}

std::vector<std::string> inip::Section::get_all_keys()
{
  std::vector<std::string> ret;

  for (auto it = this->data.begin(); it != this->data.end(); it++) {
    ret.push_back(it->first);
  }

  return ret;
}

