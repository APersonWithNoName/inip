/**
 * @file ini.hpp
 * @brief  inip Core parser interface
 * @author APersonWithNoName
 * @version 0.1.0
 * @date 2026-08-20
 */
#ifndef __INIP_INI_HPP__
#define __INIP_INI_HPP__

#include "inip/Exception.hpp"
#include "inip/IniMgr.hpp"
#include "inip/Section.hpp"

#include <cstddef>
#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace inip
{
/**
 * @brief inip Interface
 *
 * @details inip variety parser interface
 */
class ini
{
private:
  inip::IniMgr inimgr;

public:
  /**
   * @brief ini Constructor
   *
   * @param file_name ini file name
   */
  ini(const std::string &file_name = "");

  /**
   * @brief load_file Load and parse ini daya from given ini file
   *
   * @param file_name The file name
   * @param err_list A @ref inip::err::ErrCode variable
   * @param failed_skip Skip errors or stop at the first error
   * @param use_import Enable import other files
   * @param import_cover_sec_if_exist Enable cover imported content by the file
   * @param allow_duplicate_sec Allow duplicate sections
   * @param cover_sec_if_exist If a section existed, whether cover the existed section
   * @param allow_duplicate_keys Allow duplicate keys
   * @param cover_key_if_exist  If a key existed in the section, whether cover the existed key
   *
   * @return Return the latest error, or NO_ERRORS
   *
   * @details Use the given file name, instead of the file name in constructor
   */
  err::Errors load_file(const std::string &file_name,
                        std::vector<inip::err::Errors> &err_list,
                        const bool failed_skip = false,
                        const bool use_import = true,
                        const bool import_cover_sec_if_exist = true,
                        const bool allow_duplicate_sec = true,
                        const bool cover_sec_if_exist = true,
                        const bool allow_duplicate_keys = true,
                        const bool cover_key_if_exist = true);

  /**
   * @brief load_file Load and parse ini daya from constructor ini file
   *
   * @param err_list A @ref inip::err::ErrCode variable
   * @param failed_skip Skip errors or stop at the first error
   * @param use_import Enable import other files
   * @param import_cover_sec_if_exist Enable cover imported content by the file
   * @param allow_duplicate_sec Allow duplicate sections
   * @param cover_sec_if_exist If a section existed, whether cover the existed section
   * @param allow_duplicate_keys Allow duplicate keys
   * @param cover_key_if_exist  If a key existed in the section, whether cover the existed key
   *
   * @return Return the latest error, or NO_ERRORS
   *
   * @details Use the file name in constructor
   */
  err::Errors load_file(std::vector<inip::err::Errors> &err_list,
                        const bool failed_skip = false,
                        const bool use_import = true,
                        const bool import_cover_sec_if_exist = true,
                        const bool allow_duplicate_sec = true,
                        const bool cover_sec_if_exist = true,
                        const bool allow_duplicate_keys = true,
                        const bool cover_key_if_exist = true);

  /**
   * @brief load_file Load and parse ini daya from `std::string`
   *
   * @param str The given string
   * @param err_list A @ref inip::err::ErrCode variable
   * @param failed_skip Skip errors or stop at the first error
   * @param use_import Enable import other files
   * @param import_cover_sec_if_exist Enable cover imported content by the file
   * @param allow_duplicate_sec Allow duplicate sections
   * @param cover_sec_if_exist If a section existed, whether cover the existed section
   * @param allow_duplicate_keys Allow duplicate keys
   * @param cover_key_if_exist  If a key existed in the section, whether cover the existed key
   *
   * @return Return the latest error, or NO_ERRORS
   */
  err::Errors load_str(const std::string &str,
                       std::vector<inip::err::Errors> &err_list,
                       const bool failed_skip = false,
                       const bool use_import = true,
                       const bool import_cover_sec_if_exist = true,
                       const bool allow_duplicate_sec = true,
                       const bool cover_sec_if_exist = true,
                       const bool allow_duplicate_keys = true,
                       const bool cover_key_if_exist = true);

  /**
   * @brief load_file Load and parse ini daya from std::stringstream
   *
   * @param str The given stringstream
   * @param err_list A @ref inip::err::ErrCode variable
   * @param failed_skip Skip errors or stop at the first error
   * @param use_import Enable import other files
   * @param import_cover_sec_if_exist Enable cover imported content by the file
   * @param allow_duplicate_sec Allow duplicate sections
   * @param cover_sec_if_exist If a section existed, whether cover the existed section
   * @param allow_duplicate_keys Allow duplicate keys
   * @param cover_key_if_exist  If a key existed in the section, whether cover the existed key
   *
   * @return Return the latest error, or NO_ERRORS
   */
  err::Errors load_sstr(std::stringstream &str,
                        std::vector<inip::err::Errors> &err_list,
                        const bool failed_skip = false,
                        const bool use_import = true,
                        const bool import_cover_sec_if_exist = true,
                        const bool allow_duplicate_sec = true,
                        const bool cover_sec_if_exist = true,
                        const bool allow_duplicate_keys = true,
                        const bool cover_key_if_exist = true);

  /**
   * @brief contains Judge whether a section existed
   *
   * @param secname Section name
   *
   * @return If existed true, or false
   */
  bool contains(const std::string &secname) const;

  /**
   * @brief exist_key Judge whether a key existed in a non-global section
   *
   * @param secname Section name
   * @param key Key
   *
   * @return If existed true, or false
   */
  bool exist_key(const std::string &secname, const std::string &key) const;

  /**
   * @brief exist_key Judge whether a key existed in a global section
   *
   * @param key Key
   *
   * @return If existed true, or false
   */
  bool exist_key(const std::string &key) const;


  /**
   * @brief set Create a new section
   *
   * @param secname Section name
   *
   * @details If not existed, create, or skip
   */
  void set(const std::string &secname);

  /**
   * @brief set Create a new key in a global section
   *
   * @param key Key
   * @param value Value
   *
   * @details If not existed, create, or overwrite it to the new value
   */
  void set(const std::string &key, const std::string &value);

  /**
   * @brief set Create a new key in a non-global section
   *
   * @param secname Section name
   * @param key Key
   * @param value Value
   *
   * @details If not existed, create, or overwrite it to the new value
   */
  void set(const std::string &secname, const std::string &key, const std::string &value);


  /**
   * @brief get_section Get the copy of a section
   *
   * @param secname Section name
   *
   * @return @ref inip::Section
   */
  Section get_section(const std::string &secname);

  /**
   * @brief get Get a key in a global section
   *
   * @param key Key
   *
   * @return Value of the key. If not existed, throw exception
   */
  std::string get(const std::string &key) const;

  /**
   * @brief get Get a key in a non-global section
   *
   * @param secname Section name
   * @param key Key
   *
   * @return Value of the key. If not existed, throw exception
   */
  std::string get(const std::string &secname, const std::string &key) const;

  /**
   * @brief get_default Get a key in a global section with default
   *
   * @param key Key
   * @param def Default value
   *
   * @details If not existed, return the default value
   */
  std::string get_default(const std::string &key, const std::string &def) const;

  /**
   * @brief get_default Get a key in a non-global section with default
   *
   * @param secname Section name
   * @param key Key
   * @param def Default value
   *
   * @return If not existed, return the default value
   */
  std::string get_default(const std::string &secname, const std::string &key, const std::string &def) const;

  /**
   * @brief set_value Set value by a custom function convertor
   *
   * @tparam T Custom type
   * @param secname Section name
   * @param key Key
   * @param value Value
   * @param fn Custom convertor from const T to std::string
   */
  template <typename T>
  auto set_value(const std::string &secname,
                 const std::string &key,
                 const T value,
                 const std::function<std::string(const T def_value)> &fn) -> void
  {
    this->set(secname, key, fn(value));
  }

  /**
   * @brief get_value Get value by a custom function convertor
   *
   * @tparam T Custom type
   * @param secname Section name
   * @param key Key
   * @param fn Custom convertor from const T to std::string
   *
   * @details If any exceptions, throw
   */
  template <typename T>
  auto get_value(const std::string &secname,
                 const std::string &key,
                 const std::function<T(const std::string raw_str)> &fn) -> T const
  {
    return fn(this->get(secname, key));
  }

  /**
   * @brief get_value_def Get value by a custom function convertor with default
   *
   * @tparam T Custom type
   * @param def Default value
   * @param secname Section name
   * @param key Key
   * @param fn Custom convertor
   *
   * @details If any exceptions, return default value
   */
  template <typename T>
  auto get_value_def(const std::string &secname,
                     const std::string &key,
                     const std::function<T(const std::string raw_str)> &fn,
                     const T def) -> T const
  {
    try {
      const std::string str_value = this->get(secname, key);
      return fn(str_value);
    }
    catch (...) {
      return def;
    }
  }

  /**
   * @brief section_names Copy all section' names to `std::vector`
   *
   * @tparam T The container, `std::vector<std::string>>`(default) or `std::list<std::string>>`
   * @param skip_empty Whether skip the empty section
   */
  template <typename T = std::vector<std::string>>
  auto section_names(const bool skip_empty = false) -> T const
  {
    T ret;
    for (const auto &pair : this->inimgr.data) {
      if (skip_empty && pair.second.empty()) {
        continue;
      }
      ret.push_back(typename T::value_type(pair.first));
    }
    return ret;
  }

  /**
   * @brief sections Return a const reference_wrapper to `std::vector`
   *
   * @tparam T The container, `std::vector<std::reference_wrapper<const ::inip::Section>>`(default) or `std::list<std::reference_wrapper<const ::inip::Section>>`
   * @param skip_empty Whether skip the empty section
   */
  template <typename T = std::vector<std::reference_wrapper<const ::inip::Section>>>
  auto sections(const bool skip_empty = false) -> T const
  {
    T ret;
    for (const auto &pair : this->inimgr.data) {
      if (skip_empty && pair.second.empty()) {
        continue;
      }
      ret.push_back(std::cref(pair.second));
    }
    return ret;
  }

  /**
   * @brief to_string Format ini data to std::string
   */
  std::string to_string(void) const;

  /**
   * @brief clear Clear the ini data
   *
   * @param clear_name Whether clear the file name
   */
  void clear(const bool clear_name = false);

  /**
   * @brief write Write back to file
   *
   * @return Write error
   */
  err::Errors write() const;

  /**
   * @brief write Write back to another file
   *
   * @param file_name File name
   *
   * @return Write error
   */
  err::Errors write(const std::string &file_name) const;

  /**
   * @brief operator= Copy value to another ini class
   */
  ini &operator=(const ini &ini_obj);

  /// @name STL style functions
  /// @{
  /**
   * @brief operator[] Return the reference section, or a empty section
   */
  inip::Section &operator[](const std::string &sec);

  /**
   * @brief at Return the reference section, or throw a exception
   */
  inip::Section &at(const std::string &sec);

  /**
   * @brief at Return the const reference section, or throw a exception
   */
  const inip::Section &at(const std::string &sec) const;

  std::size_t size() const;
  std::size_t max_size() const;

  typedef std::map<std::string, inip::Section>::const_iterator const_iterator;
  typedef std::map<std::string, inip::Section>::const_reverse_iterator const_reverse_iterator;

  const_iterator cbegin(void) const;
  const_iterator cend(void) const;
  const_reverse_iterator crbegin(void) const;
  const_reverse_iterator crend(void) const;
  /// @}

  /**
   * @brief _INIP_GENERATE_TYPES_FUNC Internal marco of create declaration of builtin convertors
   */
#define _INIP_GENERATE_TYPES_FUNC(type_display, type_real)                                            \
  void set_##type_display(const std::string &secname, const std::string &key, const type_real value); \
  type_real get_##type_display(const std::string &secname, const std::string &key);                   \
  type_real get_##type_display##_def(const std::string &secname, const std::string &key, const type_real def);

  /// @name Type convertors
  /// @{
  _INIP_GENERATE_TYPES_FUNC(bool, bool)
  _INIP_GENERATE_TYPES_FUNC(int, int)
  _INIP_GENERATE_TYPES_FUNC(uint, unsigned int)
  _INIP_GENERATE_TYPES_FUNC(long, long)
  _INIP_GENERATE_TYPES_FUNC(double, double)
  _INIP_GENERATE_TYPES_FUNC(float, float)
  /// @}

#undef _INIP_GENERATE_TYPES_FUNC
};
} // namespace inip


#endif
