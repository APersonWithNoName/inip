#include "inip/Node.hpp"
#include "inip/Types.hpp"

#include <sstream>
#include <string>


inip::Node::Node() : line(0)
{}

inip::Node::Node(const std::string &value, const inip::Types::LineNum line) : value(value), line(line)
{}

void inip::Node::set_value(const std::string &value)
{
  this->value = value;
}

inip::Node::Node(const Node &other) = default;

std::string inip::Node::get_value() const
{
  return this->value;
}

inip::Node &inip::Node::operator=(const std::string &value)
{
  this->set_value(value);
  return *this;
}

inip::Node &inip::Node::operator=(const inip::Node &node)
{
  if (this != &node) {
    this->set_value(node.value);
    this->set_line(node.line);
  }
  return *this;
}

inip::Types::LineNum inip::Node::get_line() const
{
  return this->line;
}

void inip::Node::set_line(const inip::Types::LineNum line)
{
  this->line = line;
}

std::string inip::Node::to_string() const
{
  std::stringstream ss;
  ss << "Node{value=\"" << value << "\", line=" << line << "}";
  return ss.str();
}
