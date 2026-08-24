#include "inip/Section.hpp"
#include "inip/Exception.hpp"
#include "inip/Node.hpp"
#include "inip/Tools.hpp"
#include "inip/Types.hpp"

#include <cstddef>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>

inip::Section::Section() : line(0) {};

inip::Section::Section(const std::string &name, const std::map<std::string, Node> &data, const inip::Types::LineNum line)
{
  this->data = data;
  this->name = name;
  this->line = line;
}

bool inip::Section::contains(const std::string &key) const
{
  return this->data.find(key) != this->data.end();
}

inip::Node inip::Section::get_node(const std::string &key) const
{
  try {
    return this->data.at(key);
  }
  catch (const std::out_of_range &e) {
    throw inip::err::Errors(err::ErrCode::NO_SUCH_KEY);
  }
}

std::string inip::Section::get_value(const std::string &key) const
{
  auto node = this->get_node(key);
  return node.get_value();
}

std::string inip::Section::get_value_def(const std::string &key, const std::string &def) const
{
  try {
    return this->get_value(key);
  }
  catch (...) {
    return def;
  }
}

void inip::Section::set_line(const inip::Types::LineNum line)
{
  this->line = line;
}

void inip::Section::set(const std::string &key, const std::string &value, inip::Types::LineNum line)
{
  auto it = this->data.find(key);
  // exusted, set linenum
  if (!(it == this->data.end())) {
    it->second = value;
    it->second.set_line(line);
  }
  else {
    inip::Node node(value);
    node.set_line(line);
    this->data[key] = node;
  }
}

void inip::Section::set(const std::string &key, const std::string &value)
{
  this->data[key] = value;
}

void inip::Section::clear(const bool clear_name)
{
  this->data.clear();
  if (clear_name) {
    this->name.clear();
    this->line = 0;
  }
}


void inip::Section::insert(const std::string &key, const Node &node)
{
  this->data[key] = node;
}

void inip::Section::set_name(const std::string &name)
{
  this->name = name;
}

bool inip::Section::empty(void) const
{
  return this->data.empty();
}

std::string inip::Section::get_name(void) const
{
  return this->name;
}

inip::Types::LineNum inip::Section::get_line(void) const
{
  return this->line;
}

std::string inip::Section::to_string(void) const
{
  std::stringstream ret;

  if (!this->name.empty()) {
    ret << "[" << Tools::add_escape(this->name) << "]" << "\n";
  }

  for (const auto &i : data) {
    ret << Tools::add_escape(i.first) << " = " << Tools::add_escape(i.second.get_value()) << "\n";
  }

  return ret.str();
}

std::size_t inip::Section::size(void) const
{
  return this->data.size();
}

std::size_t inip::Section::max_size(void) const
{
  return this->data.max_size();
}

void inip::Section::erase(const std::string &key)
{
  this->data.erase(key);
}

inip::Node &inip::Section::operator[](const std::string &key)
{
  return this->data[key];
}

inip::Node &inip::Section::at(const std::string &key)
{
  auto it = data.find(key);
  if (it == data.end()) {
    throw inip::err::Errors(inip::err::ErrCode::NO_SUCH_KEY);
  }
  return it->second;
}

const inip::Node &inip::Section::at(const std::string &key) const
{
  auto it = data.find(key);
  if (it == data.end()) {
    throw inip::err::Errors(inip::err::ErrCode::NO_SUCH_KEY);
  }
  return it->second;
}

inip::Section::const_iterator inip::Section::cbegin() const
{
  return this->data.cbegin();
}

inip::Section::const_iterator inip::Section::cend() const
{
  return this->data.cend();
}

inip::Section::iterator inip::Section::begin()
{
  return this->data.begin();
};

inip::Section::iterator inip::Section::end()
{
  return this->data.end();
};


inip::Section::const_iterator inip::Section::begin() const
{
  return this->data.begin();
};

inip::Section::const_iterator inip::Section::end() const
{
  return this->data.end();
};
