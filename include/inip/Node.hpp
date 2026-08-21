/**
 * @file Node.hpp
 * @brief ini Node class
 * @author APersonWithNoName
 * @version 0.1.0
 * @date 2026-08-20
 */

#ifndef __INIP_NODE_HPP__
#define __INIP_NODE_HPP__

#include "inip/Types.hpp"

#include <string>

namespace inip
{

class Node
{
private:
  std::string value;
  inip::Types::LineNum line;

public:
  Node();
  Node(const std::string &value, const inip::Types::LineNum line = 0);
  Node(const Node &other);

  void set_line(const inip::Types::LineNum line);
  void set_value(const std::string &value);

  inip::Types::LineNum get_line() const;
  std::string get_value() const;

  std::string to_string() const;

  inip::Node &operator=(const std::string &value);
  inip::Node &operator=(const inip::Node &node);
};

} // namespace inip

#endif
