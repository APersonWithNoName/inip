/**
 * @file Section.hpp
 * @brief ini Section class
 * @author APersonWithNoName
 * @version 0.1.0
 * @date 2026-08-20
 */
#ifndef __INIP_SECTION_HPP__
#define __INIP_SECTION_HPP__

#include "inip/Node.hpp"
#include "inip/Types.hpp"

#include <cstddef>
#include <map>
#include <string>
#include <vector>

namespace inip
{

/**
 * @brief A section.of ini
 */
class Section
{
private:
  inip::Types::LineNum line;
  std::string name;
  std::map<std::string, Node> data;

public:
  /**
   * @brief Section Constructor
   */
  Section();
  /**
   * @brief Section Constructor
   *
   * @param name Section Name
   * @param data Section data as a `std::map`
   * @param line Line number
   */
  Section(const std::string &name, const std::map<std::string, Node> &data = {}, const Types::LineNum line = 0);

  /**
   * @brief contains Whether contains a key
   */
  bool contains(const std::string &key) const;

  /**
   * @brief empty If is empty
   */
  bool empty(void) const;

  /**
   * @brief get_node Get the copy of a node, or empty
   */
  Node get_node(const std::string &key) const;


  /**
   * @brief set Set the key value
   */
  void set(const std::string &key, const std::string &value);

  /**
   * @brief set Set the key value with line number
   *
   * @param line Line number
   */
  void set(const std::string &key, const std::string &value, inip::Types::LineNum line);

  /**
   * @brief get_value Get the value of key
   */
  std::string get_value(const std::string &key) const;

  /**
   * @brief get_value_def Get the value of key with default
   */
  std::string get_value_def(const std::string &key, const std::string &def) const;


  inip::Types::LineNum get_line(void) const;
  void set_line(const inip::Types::LineNum line);
  std::string get_name(void) const;
  void set_name(const std::string &name);

  /**
   * @brief to_string Format section data to string
   */
  std::string to_string(void) const;

  /**
   * @brief keys
   *
   * @tparam T The containor `std::vector<std::string>>`(drfault), or `std::list<std::string>>`
   */
  template <typename T = std::vector<std::string>>
  auto keys() const -> T
  {
    T ret;
    for (const auto &key : this->data) {
      ret.push_back(key.first);
    }
    return ret;
  }

  /**
   * @brief clear Clear this section
   *
   * @param clear_name Whether clear the section name
   */
  void clear(const bool clear_name = false);
  /// @name STL style functions
  /// @{
  std::size_t size(void) const;
  std::size_t max_size(void) const;

  void erase(const std::string &key);
  void insert(const std::string &key, const Node &node);

  inip::Node &operator[](const std::string &key);
  inip::Node &at(const std::string &key);
  const Node &at(const std::string &key) const;

  typedef std::map<std::string, inip::Node>::const_iterator const_iterator;
  typedef std::map<std::string, inip::Node>::iterator iterator;
  iterator begin(void);
  iterator end(void);
  const_iterator begin(void) const;
  const_iterator end(void) const;
  const_iterator cbegin(void) const;
  const_iterator cend(void) const;
  /// @}
};

} // namespace inip

#endif
