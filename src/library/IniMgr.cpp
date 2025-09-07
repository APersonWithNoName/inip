#include "inip/IniMgr.hpp"
#include "inip/Exception.hpp"
#include "inip/Section.hpp"

#include <ios>
#include <string>
#include <sstream>

inip::IniMgr::IniMgr(const std::string &file_name)
{
  this->file_name = file_name;
  this->file_obj.open(file_name, std::ios::in | std::ios::out);
}

inip::IniMgr::~IniMgr()
{
  this->file_obj.close();
  this->file_obj.clear();
}

bool inip::IniMgr::line_filter(const std::string &input, std::string &output)
{
  bool ret;
  char c = input.at(input.find_first_not_of(' '));
  if (c == ';' || c == '#') {
    ret = false;
  }
  else {
    ret = true;
  }

  if (ret) {
    std::string::size_type pos;
    for (std::string::size_type i = 0; 1 < input.length(); i++) {
      if (i == '#' || i == ';') {
        if (input.at(i - 1) == '\\') {
          continue;
        }
        pos = i; // 注释符位置
        break;
      }
      continue;
    }
    output = input.substr(0, pos);
  }
  else {
    output = "";
  }

  return ret;
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
  unsigned int linenum = 1;
  std::string linedata, _linedata, secname, key, value;
  inip::Section sec("");
  this->data[""] = sec;
  this->sec_list.push_back("");
  std::string::size_type linelen;

  while (std::getline(iss, linedata))
  {
    if(!this->line_filter(linedata, _linedata)) {
      linenum++;
      continue;    
    }

    linelen = _linedata.length();

    if (_linedata.at(_linedata.find_first_not_of(' ')) == '[') {
      std::string::size_type first = _linedata.find_first_of('[');
      std::string::size_type last = _linedata.find_first_of(']');
      if (last == std::string::npos) {
#ifdef INIP_THROW_EXP_IF_ERROR
        return inip::err::Errors(inip::err::ErrCode::MISS_BRACKETS, linenum, this->file_name);
#else
        throw inip::err::Errors(inip::err::ErrCode::MISS_BRACKETS, linenum, this->file_name);
#endif
      }

      secname = _linedata.substr(first, last - first +1);

#ifdef INIP_DISABLE_APPEND_SECTION
      if (this->is_section_exists(secname)) {
      #ifdef INIP_THROW_EXP_IF_ERROR
        throw inip::err::Errors(inip::err::ErrCode::SEC_EXISTS, linenum, this->file_name);
      #else
        return inip::err::Errors(inip::err::ErrCode::SEC_EXISTS, linenum, this->file_name);
      #endif
      }
#else
      if (this->is_section_exists(secname)) {
      #ifdef INIP_COVER_SECTION_IF_EXIST
        this->data[secname] = inip::Section(secname);
      #endif
      }
      else {
        this->data[secname] = inip::Section(secname);
      }
#endif
    }
    else {
      std::string::size_type equal_pos = _linedata.find('=');
      if (equal_pos == std::string::npos) {
#ifdef INIP_THROW_EXP_IF_ERROR
        throw inip::err::Errors(inip::err::ErrCode::MISS_BRACKETS, linenum, this->file_name);
#else
        return inip::err::Errors(inip::err::ErrCode::MISS_BRACKETS, linenum, this->file_name);
#endif
      }

      std::string::size_type \
      key_first_pos = 0, \
      key_last_pos = 0, \
      value_first_pos = 0, \
      value_last_pos = 0;

      // 计算键，值的起始与结束位置
      for (std::string::size_type i = equal_pos; i >= 0; i--) {
        if (linedata[i] == ' ')
          continue;
        else {
          key_last_pos = i;
          break;
        }
      }
      for (std::string::size_type i = 0; i < equal_pos; i++) {
        if (linedata[i] == ' ')
          continue;
        else {
          key_first_pos = i;
          break;
        }
      }
      for (std::string::size_type i = equal_pos; i < linelen; i++) {
        if(linedata[i] == ' ')
          continue;
        else {
          value_first_pos = i;
          break;
        }
      }
      for (std::string::size_type i = linelen - 1; i > equal_pos; i--) {
        if (linedata[i] == ' ')
          continue;
        else {
          value_last_pos = i;
          break;
        }
      }
      // 提取键值对
      key = linedata.substr(key_first_pos, key_last_pos - key_first_pos);
      value = linedata.substr(value_first_pos + 1, value_last_pos - value_first_pos + 1);

      if (key == "") {
#ifdef INIP_THROW_EXP_IF_ERROR
      throw inip::err::Errors(inip::err::ErrCode::KEY_EMPTY, linenum, this->file_name);
#else
      return inip::err::Errors(inip::err::ErrCode::KEY_EMPTY, linenum, this->file_name);
#endif
      }
#ifdef INIP_USE_IMPORT
      // 指定引入 Section, node尚未实现
      if (key == "__import__") {
        if (secname == "") {
        #ifdef INIP_THROW_EXP_IF_ERROR
          try {
            inip::IniMgr inimgr(value);
            inimgr.parse_file();
            this->data.insert(inimgr.data.begin(), inimgr.data.end());
          }
          catch (...) {
            throw inip::err::Errors(inip::err::ErrCode::IMPORT_ERROR);
          }
        #else
          inip::IniMgr inimgr(value);
          if (!(inimgr.parse_file().code == inip::err::ErrCode::NO_ERRORS))
            return inip::err::Errors(inip::err::ErrCode::IMPORT_ERROR);
          this->data.insert(inimgr.data.begin(), inimgr.data.end());
        #endif
        }
        else {
        #ifdef INIP_THROW_EXP_IF_ERROR
          throw inip::err::Errors(inip::err::ErrCode::IMPORT_INVALID_LOCATION);
        #else
          return inip::err::Errors(inip::err::ErrCode::IMPORT_INVALID_LOCATION);
        #endif
        }
      }
#endif

      if (this->data[secname].is_key_exist(key)) {
#ifdef INIP_DISABLE_APPEND_SECTION
      #ifdef INIP_THROW_EXP_IF_ERROR
        throw inip::err::Errors(inip::err::ErrCode::KEY_EXISTS, linenum, this->file_name);
      #else
        return inip::err::Errors(inip::err::ErrCode::KEY_EXISTS, linenum, this->file_name);
      #endif
#else
        this->data[secname].add(key, value);
#endif
      }
    }
    linenum++;
  }

  return inip::err::Errors(inip::err::ErrCode::NO_ERRORS, linenum, file_name);
}

inip::err::Errors inip::IniMgr::parse_file()
{
  if (!this->file_obj.is_open()) {
#ifdef INIP_THROW_EXP_IF_ERROR
    throw inip
#endif
  }
}
