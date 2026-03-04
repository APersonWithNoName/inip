/**
 * @file IniMgr.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-02-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef __INIP_INIMGR_HPP__
#define __INIP_INIMGR_HPP__

#include "inip/Exception.hpp"
#include "inip/Section.hpp"

#include <string>
#include <sstream>
#include <vector>
#include <map>

namespace inip {

/**
 * @brief Ini解析器类
 *
 * 提供了所有关于ini解析的函数
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

  const bool line_filter(const std::string &input, std::string &output);

  /**
   * @brief 从一个istreamstring读取并解析ini数据
   * 
   * @param iss 输入字符串
   * @param err_list 错误列表
   * @param failed_skip 是否跳过错误: 若为true, 则出现错误时跳过并继续解析, 错误将写入err_list; 若false就返回错误
   * @param use_import 是否允许"__import__"关键字扩展
   * @param import_cover_sec_if_exist 导入时, 如果出现同名节时, 是否完全覆盖(清空原有节数据)
   * @param allow_duplicate_sec 是否允许重复节
   * @param cover_sec_if_exist 如果出现同名节时, 是否完全覆盖(清空原有节数据), true则完全覆盖, false则只追加, 如果出现相同键, 参见allow_duplicate_keys
   * @param allow_duplicate_keys 是否允许重复键(在同一节中)
   * @param cover_key_if_exist 如果出现同名键时, 是否覆盖, true则覆盖, false则只保持原有值不变
   * @return err::Errors 返回最后一个错误, 若无返回NO_ERRORS
   */
  err::Errors parse_str(std::istringstream &iss,
    std::vector<inip::err::Errors> &err_list, const bool failed_skip = false,
    const bool use_import=true, const bool import_cover_sec_if_exist=true,
    const bool allow_duplicate_sec=true, const bool cover_sec_if_exist=true,
    const bool allow_duplicate_keys=true, const bool cover_key_if_exist=true);
  err::Errors parse_file(std::vector<inip::err::Errors> &err_list, const bool failed_skip = false,
    const bool use_import=true, const bool import_cover_sec_if_exist=true,
    const bool allow_duplicate_sec=true, const bool cover_sec_if_exist=true,
    const bool allow_duplicate_keys=true, const bool cover_key_if_exist=true);
  err::Errors parse_file(const std::string &file_name,
    std::vector<inip::err::Errors> &err_list, const bool failed_skip = false,
    const bool use_import=true, const bool import_cover_sec_if_exist=true,
    const bool allow_duplicate_sec=true, const bool cover_sec_if_exist=true,
    const bool allow_duplicate_keys=true, const bool cover_key_if_exist=true);

  unsigned int get_line(const std::string &secname) const;
  unsigned int get_line(const std::string &secname, const std::string &key) const;
};

}

#endif
