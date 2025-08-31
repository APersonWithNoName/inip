#ifndef __INIP_NODE_HPP__
#define __INIP_NODE_HPP__

#include <string>

namespace inip {

class Node
{
private:
  std::string value;

public:
	Node();
  Node(const std::string &value);
  void set_value(const std::string &value);
  std::string get_value();
  std::string to_string();
};

}

#endif
