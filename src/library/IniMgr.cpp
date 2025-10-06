#include "inip/IniMgr.hpp"
#include "inip/Exception.hpp"
#include "inip/Section.hpp"
#include "inip/Tools.hpp"

#include "config.h"

#include <fstream>
#include <ios>
#include <string>
#include <sstream>

inip::IniMgr::IniMgr(const std::string &file_name)
{
  this->file_name = file_name;
}

inip::IniMgr::~IniMgr() {}

bool inip::IniMgr::line_filter(const std::string &input, std::string &output)
{
  // 去除每行前后空格
  std::string tmp = inip::Tools::trim(input);
  if (tmp == "") {
    output = "";
    return false; // 空行
  }
  // 判断注释行
  char c = tmp[0];
  if (c == ';' || c == '#') {
    output = "";
    return false; // 注释行
  }
  // 查找注释符位置
  std::string::size_type pos = 0;
  for (std::string::size_type i = 1; i < tmp.length(); i++) {
    char c = tmp.at(i);
    // 出现注释符
    if (c == ';' || c == '#') {
      // 判断注释符前是否转义
      if (tmp.at(i - 1) == '\\') {
        continue;
      }
      // 注释符位置
      pos = i;
      break;
    }
    pos = input.length();
  }

  output = tmp.substr(0, pos);
  return true; // 非注释行
}

bool inip::IniMgr::is_section_exists(const std::string &secname)
{
  if (this->data.find(secname) == this->data.end()) {
    return false;
  }
  return true;
}

inip::err::Errors inip::IniMgr::parse_str(std::istringstream &iss)
{
  // 当前行号
  unsigned int linenum = 1;
  // 读取的原始行内容 剥除前后空白字符的行行内容 节名 键 值
  std::string linedata, _linedata, secname, key, value;
  // 空白的节实例
  inip::Section sec("");
  // 为当前 data 添加空白实例
  this->data[""] = sec;
  // 向 sec_list 添加节点名
  inip::Tools::append_sec_value<std::string>(this->sec_list, "");
  // 每行长度
  std::string::size_type linelen;
  // 全局错误实例
  inip::err::Errors err(inip::err::ErrCode::NO_ERRORS, linenum, file_name);

  while (std::getline(iss, linedata))
  {
    // 截去注释和前后空格
    if(!this->line_filter(linedata, _linedata)) {
      linenum++;
      continue;    
    }

    linelen = _linedata.length();

    // 处理节名称行
    if (!_linedata.empty() && _linedata[0] == '[') {
      // 查找 [] 的起始位置
      std::string::size_type first = _linedata.find_first_of('[');
      std::string::size_type last = _linedata.find_first_of(']');
      // 不存在 ]
      if (last == std::string::npos) {
        err.set_code(inip::err::ErrCode::MISS_BRACKETS).set_line(linenum).set_file(this->file_name);
        goto ret_err;
      }

      // 读取节名称
      secname = _linedata.substr(first + 1, last - first - 1);
      // 节名称为空
      if (secname.empty()) {
        err.set_code(inip::err::ErrCode::SEC_EMPTY).set_line(linenum).set_file(this->file_name);
        goto ret_err;
      }
      // 出现重复节
      if (this->is_section_exists(secname)) {
        // 允许节重复
#ifdef INIP_ALLOW_DUPLICATE_SECTIONS
        // 重复则覆盖
      #ifdef INIP_COVER_SECTION_IF_EXIST
        // 先清除原有节内容
        this->data[secname].clear();
        // 重复则追加
      #endif
        // 不允许重复
#else
        err.set_code(inip::err::ErrCode::SEC_EXISTS).set_line(linenum).set_file(this->file_name);
        goto ret_err;
#endif
      }
      // 节未重复，添加新节
      else {
        // 创建实例
        this->data[secname] = inip::Section(secname);
        // 添加名称到 sec_list
        inip::Tools::append_sec_value<std::string>(this->sec_list, secname);
      }
    }
    // 处理键值对行
    else {
      // 查找 ‘=’ 位置
      // 如果没有 key 则等号在 0 位置即 
      // = value
      //  ^ 从此开始查找
      std::string::size_type equal_pos = 0;
      for (std::string::size_type i = 1; i < linelen; i++) {
        if (_linedata[i] == '=') {
          if (_linedata[i - 1] == '\\') continue;
          else {
            equal_pos = i;
            break;
          }
        }
      }

      if (equal_pos == 0) {
        err.set_code(inip::err::ErrCode::MISS_EQUAL).set_line(linenum).set_file(this->file_name);
        goto ret_err;
      }

      // 截取键值对
      //           key_last = equal_pos - 1 ^
      // _linedata: kkkkkkkkkkkkkkkkkkk = vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
      //            ^                   ^                                  ^
      // position:  0                                                      linelen - 1
      //            Key_first           equal_pos                          value_last
      //      key_last = equal_pos - 1 ^ ^ value_first = equal_pos + 1
      // length     |<- equal_pos     ->|<- value_last - value_first + 1 ->|
      //                                    = linelen - 1 - (equal_pos + 1) + 1
      key = inip::Tools::trim(_linedata.substr(0, equal_pos));
      value = inip::Tools::trim(_linedata.substr(equal_pos + 1, linelen - equal_pos));

      if (key == "") {
        err.set_code(inip::err::ErrCode::KEY_EMPTY).set_line(linenum).set_file(this->file_name);
        goto ret_err;
      }

#ifdef INIP_USE_IMPORT
      // 指定引入 Section, node尚未实现
      if (key == "__import__") {
        if (secname == "") {
          inip::IniMgr inimgr(value);
        #ifdef INIP_THROW_EXP_IF_ERROR
          try {
            inimgr.parse_file();
          }
          catch (inip::err::Errors &e) {
            err = e;
            goto ret_err;
          }
          catch (...) {
            err.set_code(inip::err::ErrCode::IMPORT_ERROR).set_line(linenum).set_file(this->file_name);
            goto ret_err;
          }
        #else
          inip::err::Errors result = inimgr.parse_file();
          if (result.code != inip::err::ErrCode::NO_ERRORS) {
            err = result;
            goto ret_err;
          }
        #endif
          // 向 sec_list 追加
          inip::Tools::append_sec<std::string>(this->sec_list, inimgr.sec_list);
        #ifdef INIP_IMPORT_COVER_SECTION_IF_EXIST
          // 插入并覆盖
          inip::Tools::cover_map<std::string, inip::Section>(this->data, inimgr.data);
        #else
          // 插入但不覆盖
          this->data.insert(inimgr.data.begin(), inimgr.data.end());
        #endif
        }
        else {
          err.set_code(inip::err::ErrCode::IMPORT_INVALID_LOCATION).set_line(linenum).set_file(this->file_name);
          goto ret_err;
        }
      }
#endif

      // 出现重复 Node
      if (this->data[secname].is_key_exist(key)) {
        // 允许重复
#ifdef INIP_ALLOW_DUPLICATE_KEYS
        // 重复则覆盖
      #ifndef INIP_COVER_KEY_IF_EXIST
        this->data[secname].set(key, value);
      #endif
        // 不允许重复
#else
        err.set_code(inip::err::ErrCode::KEY_EXISTS).set_line(linenum).set_file(this->file_name);
        goto ret_err;
#endif
      }
      //不重复，正常添加 Node
      else {
        this->data[secname].add(key, value);
      }
    }
    linenum++;
  }
  // 错误处理
ret_err:
#ifdef INIP_THROW_EXP_IF_ERROR
  if (err.get_code_err() != inip::err::ErrCode::NO_ERRORS){
    throw err;
  }
#endif
  return inip::err::Errors(inip::err::ErrCode::NO_ERRORS, linenum, file_name);
}

inip::err::Errors inip::IniMgr::parse_file(const std::string &file_name)
{
  this->file_obj.open(file_name, std::ios::in | std::ios::binary);

  if (!this->file_obj.is_open()) {
#ifdef INIP_THROW_EXP_IF_ERROR
    throw inip::err::Errors(inip::err::ErrCode::CANNOT_OPEN_FILE, 0, this->file_name);
#else
    return inip::err::Errors(inip::err::ErrCode::CANNOT_OPEN_FILE, 0, this->file_name);
#endif
  }

  // 从 fstream 读取到 istringstream
  std::ostringstream oss;
  oss << this->file_obj.rdbuf();
  this->file_obj.close();

  std::istringstream iss(oss.str());
  return this->parse_str(iss);
}

inip::err::Errors inip::IniMgr::parse_file()
{
  return this->parse_file(this->file_name);
}
