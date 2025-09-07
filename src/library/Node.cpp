#include "inip/Node.hpp"


inip::Node::Node()
{
  this->value = "";
}

inip::Node::Node(const std::string &value)
{
  this->value = value;
}

void inip::Node::set_value(const std::string &value)
{
  this->value = value;
}

std::string inip::Node::get_value()
{
  return this->value;
}

std::string inip::Node::operator=(const std::string &value)
{
  this->set_value(value);
  return value;
}

inip::Node inip::Node::operator=(const inip::Node &node)
{
  this->set_value(node.value);
  return node;
}
