#include "inip/Node.hpp"

#include <sstream>

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
