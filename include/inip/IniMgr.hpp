/**
 * @file IniMgr.hpp
 * @brief inip Core parser
 * @author APersonWithNoName
 * @version 0.1.0
 * @date 2026-08-20
 */

#ifndef __INIP_INIMGR_HPP__
#define __INIP_INIMGR_HPP__

#include "inip/Exception.hpp"
#include "inip/Section.hpp"

#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace inip
{

/**
 * @brief inip Core parser
 */
class IniMgr
{
private:
  std::string file_name;
  std::map<std::string, Section> data;
  std::vector<std::string> sec_list;
  friend class ini;

public:
  IniMgr(const std::string &file_name = "");
  ~IniMgr();

  bool contains(const std::string &secname) const;

  static const bool line_filter(const std::string &input, std::string &output);
  static const bool check_escape(const std::string &s);
  static std::string::size_type find_first_keyword(const std::string &s, const char c);

  err::Errors parse_str(std::istringstream &iss,
                        std::vector<inip::err::Errors> &err_list,
                        const bool failed_skip = false,
                        const bool use_import = true,
                        const bool import_cover_sec_if_exist = true,
                        const bool allow_duplicate_sec = true,
                        const bool cover_sec_if_exist = true,
                        const bool allow_duplicate_keys = true,
                        const bool cover_key_if_exist = true);
  err::Errors parse_file(std::vector<inip::err::Errors> &err_list,
                         const bool failed_skip = false,
                         const bool use_import = true,
                         const bool import_cover_sec_if_exist = true,
                         const bool allow_duplicate_sec = true,
                         const bool cover_sec_if_exist = true,
                         const bool allow_duplicate_keys = true,
                         const bool cover_key_if_exist = true);
  err::Errors parse_file(const std::string &file_name,
                         std::vector<inip::err::Errors> &err_list,
                         const bool failed_skip = false,
                         const bool use_import = true,
                         const bool import_cover_sec_if_exist = true,
                         const bool allow_duplicate_sec = true,
                         const bool cover_sec_if_exist = true,
                         const bool allow_duplicate_keys = true,
                         const bool cover_key_if_exist = true);

  unsigned int get_line(const std::string &secname) const;
  unsigned int get_line(const std::string &secname, const std::string &key) const;

  const std::map<std::string, Section> *get_data(void) const;
};

} // namespace inip

#endif
