#include "inip/IniMgr.hpp"
#include "inip/Exception.hpp"
#include "inip/Section.hpp"
#include "inip/Tools.hpp"

#include <fstream>
#include <ios>
#include <string>
#include <sstream>

inip::IniMgr::IniMgr(const std::string &file_name)
{
  this->file_name = file_name;
}

inip::IniMgr::~IniMgr() {}

const bool inip::IniMgr::line_filter(const std::string &input, std::string &output)
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
  std::string::size_type pos = tmp.length();
  for (std::string::size_type i = 0; i < tmp.length(); i++) {
    char c = tmp[i];
    // 出现注释符
    if (c == ';' || c == '#') {
      // 判断注释符前是否转义
      if (i > 0 && tmp[i - 1] == '\\') {
        // 检查是否是双反斜杠转义(\\; 不应该被转义)
        if (i > 1 && tmp[i - 2] == '\\') {
          // \\; 的情况,第一个\转义第二个\,第三个;是注释符
          pos = i;
          break;
        }
        // 单个反斜杠转义,继续查找
        continue;
      }
      // 注释符位置
      pos = i;
      break;
    }
  }

  output = inip::Tools::trim(tmp.substr(0, pos));
  return !output.empty(); // 非注释行
}

bool inip::IniMgr::contains(const std::string &secname) const
{
  return this->data.find(secname) != this->data.end();
}

inip::err::Errors inip::IniMgr::parse_str(std::istringstream &iss,
  std::vector<inip::err::Errors> &err_list, const bool failed_skip,
  const bool use_import, const bool import_cover_sec_if_exist,
  const bool allow_duplicate_sec, const bool cover_sec_if_exist,
  const bool allow_duplicate_keys, const bool cover_key_if_exist)
{
  // 当前行号
  inip::Types::LineNum linenum = 1;
  // 读取的原始行内容 剥除前后空白字符的行行内容 节名 键 值
  std::string linedata, _linedata, secname;
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
        err_list.push_back(err);
        if (!failed_skip) return err;
        else {
          linenum++;
          continue;
        }
      }

      // 读取节名称
      secname = _linedata.substr(first + 1, last - first - 1);
      secname = inip::Tools::trim(secname); // 确保节名去除空格

      // 节名称为空
      if (secname.empty()) {
        err.set_code(inip::err::ErrCode::SEC_EMPTY).set_line(linenum).set_file(this->file_name);
        err_list.push_back(err);
        if (!failed_skip) return err;
        else {
          linenum++;
          continue;
        }
      }

      // 出现重复节
      if (this->contains(secname)) {
        if (allow_duplicate_sec) {
          if (cover_sec_if_exist) {
            this->data[secname].clear();
            this->data[secname].set_line(linenum);
          }
        }
        else {
          err.set_code(inip::err::ErrCode::SEC_EXISTS).set_line(linenum).set_file(this->file_name);
          err_list.push_back(err);
          if (!failed_skip) return err;
          else {
            linenum++;
            continue;
          }
        }
      }
      // 节未重复,添加新节
      else {
        // 创建实例
        this->data[secname] = inip::Section(secname);
        // 添加名称到 sec_list
        inip::Tools::append_sec_value<std::string>(this->sec_list, secname);
        // 添加 Section 的行号
        this->data[secname].set_line(linenum);
      }
    }
    // 处理键值对行
    else {
      // 查找 ‘=’ 位置
      // 如果没有 key 则等号在 0 位置即 
      // = value
      //  ^ 从此开始查找
      std::string::size_type equal_pos = std::string::npos;
      for (std::string::size_type i = 0; i < linelen; i++) {
        if (_linedata[i] == '=') {
          // 检查转义
          if (i > 0 && _linedata[i - 1] == '\\') {
            // 检查是否是双反斜杠
            if (i > 1 && _linedata[i - 2] == '\\') {
              equal_pos = i;  // \\= 的情况,=是有效的分隔符
              break;
            }
            // 单个反斜杠转义,继续查找
            continue;
          }
          equal_pos = i;
          break;
        }
      }
      if (equal_pos == std::string::npos) {
        err.set_code(inip::err::ErrCode::MISS_EQUAL).set_line(linenum).set_file(this->file_name);
        err_list.push_back(err);
        if (!failed_skip) return err;
        else {
          linenum++;
          continue;
        }
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
      std::string raw_key = inip::Tools::trim(_linedata.substr(0, equal_pos));
      // 处理转义的等号(将\=替换为=)
      std::string processed_key;
      for (std::string::size_type i = 0; i < raw_key.length(); i++) {
        if (raw_key[i] == '\\' && i + 1 < raw_key.length() && raw_key[i + 1] == '=') {
          // 跳过反斜杠,只添加等号
          processed_key += '=';
          i++; // 跳过等号
        } else {
          processed_key += raw_key[i];
        }
      }
      std::string key = inip::Tools::trim(processed_key);

      std::string raw_value = inip::Tools::trim(_linedata.substr(equal_pos + 1, linelen - equal_pos));
      // 同样处理值中的转义等号
      std::string processed_value;
      for (size_t i = 0; i < raw_value.length(); i++) {
        if (raw_value[i] == '\\' && i + 1 < raw_value.length() && raw_value[i + 1] == '=') {
          processed_value += '=';
          i++; // 跳过等号
        } else {
          processed_value += raw_value[i];
        }
      }
      std::string value = inip::Tools::trim(processed_value);

      if (key == "") {
        err.set_code(inip::err::ErrCode::KEY_EMPTY).set_line(linenum).set_file(this->file_name);
        err_list.push_back(err);
        if (!failed_skip) return err;
        else {
          linenum++;
          continue;
        }
      }
/*
      if (use_import) {
        if (key == "__import__") {
          if (secname != "") {
            err.set_code(inip::err::ErrCode::IMPORT_INVALID_LOCATION).set_line(linenum).set_file(this->file_name);
            return err;
          }
          inip::IniMgr inimgr(value);
          inip::err::Errors result = inimgr.parse_file();
          if (result.get_code_err() != inip::err::ErrCode::NO_ERRORS) {
            err = result;
            return err;
          }
          // 向 sec_list 追加
          inip::Tools::append_sec<std::string>(this->sec_list, inimgr.sec_list);
          if (import_cover_sec_if_exist) {
            inip::Tools::cover_map<std::string, inip::Section>(this->data, inimgr.data);
          }
          else {
            this->data.insert(inimgr.data.begin(), inimgr.data.end());
          }
        }
      }
*/
      // 出现重复 Node
      if (this->data[secname].contains(key)) {
        if (allow_duplicate_keys) {
          if (cover_key_if_exist) {
            this->data[secname].set(key, value);
            this->data[secname].get_node(key).set_line(linenum);
          }
        }
        else {
          err.set_code(inip::err::ErrCode::KEY_EXISTS).set_line(linenum).set_file(this->file_name);
          err_list.push_back(err);
          if (!failed_skip) return err;
          else {
            linenum++;
            continue;
          }
        }
      }
      //不重复,正常添加 Node
      else {
        this->data[secname].set(key, value);
        // 添加 Node 行号
        this->data[secname].get_node(key).set_line(linenum);
      }
    }
    linenum++;
  }

  // 设定返回值为最后一个错误
  if (err_list.size() > 0) {
    err = err_list.back();
  }
  // 错误处理
  return err;
}

inip::err::Errors inip::IniMgr::parse_file(const std::string &file_name,
  std::vector<inip::err::Errors> &err_list, const bool failed_skip,
  const bool use_import, const bool import_cover_sec_if_exist,
  const bool allow_duplicate_sec, const bool cover_sec_if_exist,
  const bool allow_duplicate_keys, const bool cover_key_if_exist)
{
  std::ifstream file_obj;
  file_obj.open(file_name, std::ios::in | std::ios::binary);

  if (!file_obj.is_open()) {
    return inip::err::Errors(inip::err::ErrCode::CANNOT_OPEN_FILE, 0, this->file_name);
  }

  // 从 fstream 读取到 istringstream
  std::ostringstream oss;
  oss << file_obj.rdbuf();
  file_obj.close();

  std::istringstream iss(oss.str());

  return this->parse_str(iss,
    err_list, failed_skip,
    use_import, import_cover_sec_if_exist,
    allow_duplicate_sec, cover_sec_if_exist,
    allow_duplicate_keys, cover_key_if_exist);
}

inip::err::Errors inip::IniMgr::parse_file(std::vector<inip::err::Errors> &err_list, const bool failed_skip,
  const bool use_import, const bool import_cover_sec_if_exist,
  const bool allow_duplicate_sec, const bool cover_sec_if_exist,
  const bool allow_duplicate_keys, const bool cover_key_if_exist)
{
  return this->parse_file(this->file_name,
  err_list, failed_skip,
  use_import, import_cover_sec_if_exist,
  allow_duplicate_sec, cover_sec_if_exist,
  allow_duplicate_keys, cover_key_if_exist);
}

unsigned int inip::IniMgr::get_line(const std::string &secname) const
{
  if (this->contains(secname)) {
    return this->data.at(secname).get_line();
  }
  return 0;
}

unsigned int inip::IniMgr::get_line(const std::string &secname, const std::string &key) const
{
  if (this->contains(secname)) {
    if (this->data.at(secname).contains(key)) {
      return this->data.at(secname).get_node(key).get_line();
    }
    return 0;
  }
  return 0;
}
