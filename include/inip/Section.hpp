#ifndef __INIP_SECTION_HPP__
#define __INIP_SECTION_HPP__

#include "inip/Node.hpp"
#include "inip/Types.hpp"

#include <string>
#include <vector>
#include <map>

namespace inip {

class Section
{
private:
  inip::Types::LineNum line;
  std::string name;
  std::map<std::string, Node> data;

public:
  typedef std::map<std::string, inip::Node>::iterator iterator;
  typedef std::map<std::string, inip::Node>::const_iterator const_iterator;

  Section();
  Section(const std::string &name, const std::map<std::string, Node> &data = {}, const Types::LineNum line = 0);
  
  bool contains(const std::string &key) const;
  bool empty() const;

  inip::Node& operator[] (const std::string &key);
  inip::Node& at(const std::string &key);
  const Node& at(const std::string &key) const;
  
  Node get_node(const std::string &key) const;
  void set(const std::string &key, const std::string &value);
  void set(const std::string &key, const std::string &value, inip::Types::LineNum line);

  
  std::string get_value(const std::string &key) const;
  std::string get_value_def(const std::string &key, const std::string &def) const;
  void erase(const std::string &key);
  void insert(const std::string &key, const Node &node);
  
  inip::Types::LineNum get_line() const;
  void set_line(const inip::Types::LineNum);
  std::string get_name() const;
  void set_name(const std::string &name);
  
  std::string to_string() const;

  template <typename T = std::vector<std::string>>
  auto keys() const -> T
  {
    T ret;
    for (const auto &key : this->data) {
      ret.push_back(key.first);
    }
    return ret;
  }

  void clear(const bool clear_name=false);
  
  std::size_t size() const;
  std::size_t max_size() const;

  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;
  const_iterator cbegin() const;
  const_iterator cend() const;

};

}

#endif
