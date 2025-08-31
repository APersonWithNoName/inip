#ifndef __INIP_SECTION_HPP__
#define __INIP_SECTION_HPP__

#include "inip/Node.hpp"

#include <string>
#include <vector>
#include <map>

namespace inip {

class Section
{
private:
  std::string name;
  std::map<std::string, Node> data;

public:
  Section();
  Section(const std::string &name, const std::map<std::string, Node> &data = {});
  bool is_key_exist(const std::string &key);
  Node* get_node_ptr(const std::string &key);
  Node get_node(const std::string &key);
  std::string get_value(const std::string &key);
  std::string get_value_default(const std::string &key, const std::string &def);
  std::string get_name();
  void add(const std::string &key, const std::string &value);
  void set(const std::string &key, const std::string &value);
  void clear();
  bool empty();
  std::string to_string();
  std::vector<std::string> get_all_keys();
};

}

#endif
