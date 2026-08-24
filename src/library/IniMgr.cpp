#include "inip/IniMgr.hpp"
#include "inip/Exception.hpp"
#include "inip/Section.hpp"
#include "inip/Tools.hpp"
#include "inip/Types.hpp"

#include <fstream>
#include <ios>
#include <map>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>


inip::IniMgr::IniMgr(const std::string &file_name)
{
  this->file_name = file_name;
}

inip::IniMgr::~IniMgr() = default;

const bool inip::IniMgr::line_filter(const std::string &input, std::string &output)
{
  std::string tmp = inip::Tools::trim(input);
  if (tmp.empty()) {
    output = "";
    return false;
  }

  const char first_char = tmp[0];
  if (first_char == ';' || first_char == '#') {
    output = "";
    return false;
  }

  // Find first not escaped char
  std::string::size_type comment_pos = std::string::npos;
  for (std::string::size_type i = 0; i < tmp.length(); i++) {
    if ((tmp[i] == ';' || tmp[i] == '#') && i > 0 && tmp[i - 1] == '\\') {
      continue;
    }
    if (tmp[i] == ';' || tmp[i] == '#') {
      comment_pos = i;
      break;
    }
  }

  if (comment_pos != std::string::npos) {
    output = inip::Tools::trim(tmp.substr(0, comment_pos));
  }
  else {
    output = inip::Tools::trim(tmp);
  }

  return !output.empty() || (first_char != ';' && first_char != '#');
}

const bool inip::IniMgr::check_escape(const std::string &s)
{
  if (s.empty())
    return false;

  std::string::size_type espace_count = 0;
  for (std::string::size_type i = s.size() - 1; i >= 0; i--) {
    if (s[i] == '\\') {
      espace_count++;
    }
    else {
      break;
    }
  }

  return espace_count % 2 == 1;
}

std::string::size_type inip::IniMgr::find_first_keyword(const std::string &s, const char c)
{
  if (s.empty())
    return std::string::npos;

  for (std::string::size_type i = 0; i < s.size(); i++) {
    if (s[i] == c) {
      const std::string before = s.substr(0, i);
      if (!check_escape(before)) {
        return i;
      }
    }
  }

  return std::string::npos;
}

bool inip::IniMgr::contains(const std::string &secname) const
{
  return this->data.find(secname) != this->data.end();
}

inip::err::Errors inip::IniMgr::parse_str(std::istringstream &iss,
                                          std::vector<inip::err::Errors> &err_list,
                                          const bool failed_skip,
                                          const bool use_import,
                                          const bool import_cover_sec_if_exist,
                                          const bool allow_duplicate_sec,
                                          const bool cover_sec_if_exist,
                                          const bool allow_duplicate_keys,
                                          const bool cover_key_if_exist)
{
  inip::Types::LineNum linenum = 1;
  std::string linedata, _linedata, secname;
  // Default global section
  inip::Section sec("");
  this->data[""] = sec;
  inip::Tools::append_sec_value<std::string>(this->sec_list, "");
  std::string::size_type linelen;
  inip::err::Errors err(inip::err::ErrCode::NO_ERRORS, linenum, file_name);
  int skip_lines = 0;

  while (std::getline(iss, linedata)) {
    if (skip_lines > 0) {
      skip_lines--;
      linenum++;
      continue;
    }

    // Remove whitespaces and comment
    if (!line_filter(linedata, _linedata)) {
      linenum++;
      continue;
    }

    if (_linedata.empty()) {
      linenum++;
      continue;
    }

    linelen = _linedata.length();

    // found section
    if (_linedata[0] == '[') {
      // find []
      const std::string::size_type first = find_first_keyword(_linedata, '[');
      const std::string::size_type last = find_first_keyword(_linedata, ']');
      // ] not found
      if (last == std::string::npos) {
        err.set_code(inip::err::ErrCode::MISS_RIGHT_BRACKETS).set_line(linenum).set_file(this->file_name);
        err_list.push_back(err);
        if (!failed_skip)
          return err;
        linenum++;
        continue;
      }

      // get section name
      secname = _linedata.substr(first + 1, last - first - 1);
      secname = inip::Tools::trim(secname);

      // name empty
      if (secname.empty()) {
        err.set_code(inip::err::ErrCode::SEC_EMPTY).set_line(linenum).set_file(this->file_name);
        err_list.push_back(err);
        if (!failed_skip)
          return err;
        linenum++;
        continue;
      }

      // section duplicate
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
          if (!failed_skip)
            return err;
          linenum++;
          continue;
        }
      }
      // not duplicate, create new
      else {
        this->data[secname] = inip::Section(secname);
        inip::Tools::append_sec_value<std::string>(this->sec_list, secname);
        this->data[secname].set_line(linenum);
      }
    }
    // parse node
    else {
      // find =
      // If key not found, = at pos 0
      // = value
      //  ^ Start here
      const std::string::size_type equal_pos = find_first_keyword(_linedata, '=');
      if (equal_pos == std::string::npos) {
        err.set_code(inip::err::ErrCode::MISS_EQUAL).set_line(linenum).set_file(this->file_name);
        err_list.push_back(err);
        if (!failed_skip)
          return err;
        linenum++;
        continue;
      }

      //           key_last = equal_pos - 1 ^
      // _linedata: kkkkkkkkkkkkkkkkkkk = vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
      //            ^                   ^                                  ^
      // position:  0                                                      linelen - 1
      //            Key_first           equal_pos                          value_last
      //      key_last = equal_pos - 1 ^ ^ value_first = equal_pos + 1
      // length     |<- equal_pos     ->|<- value_last - value_first + 1 ->|
      //                                    = linelen - 1 - (equal_pos + 1) + 1
      std::string key = inip::Tools::trim(inip::Tools::remove_escape(_linedata.substr(0, equal_pos)));
      std::string value = inip::Tools::trim(inip::Tools::remove_escape(_linedata.substr(equal_pos + 1, linelen - equal_pos)));

      if (key.empty()) {
        err.set_code(inip::err::ErrCode::KEY_EMPTY).set_line(linenum).set_file(this->file_name);
        err_list.push_back(err);
        if (!failed_skip)
          return err;
        linenum++;
        continue;
      }

      if (linedata.back() == '\\' && !check_escape(linedata.empty() ? "" : // not escaped
                                                     linedata.substr(0, linedata.length() - 1))) {
        std::string long_str;
        std::string tmp;
        while (std::getline(iss, tmp)) {
          if (tmp.empty()) {
            skip_lines++;
            break;
          }

          auto ctx = tmp.substr(0, tmp.length() - 1);

          // not ended by backescape or be escaped
          if (tmp.back() != '\\' || check_escape(ctx)) {
            long_str += tmp;
            skip_lines++;
            break;
          }

          long_str += ctx;
          skip_lines++;
        }

        value.pop_back();
        value += long_str;
      }

      skip_lines--;

      // duplicate node
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
          if (!failed_skip)
            return err;
          linenum++;
          continue;
        }
      }
      // not duplicate, create new
      else {
        this->data[secname].set(key, value);
        this->data[secname].get_node(key).set_line(linenum);
      }
    }
    linenum++;
  }

  if (!err_list.empty()) {
    err = err_list.back();
  }

  return err;
}

inip::err::Errors inip::IniMgr::parse_file(const std::string &file_name,
                                           std::vector<inip::err::Errors> &err_list,
                                           const bool failed_skip,
                                           const bool use_import,
                                           const bool import_cover_sec_if_exist,
                                           const bool allow_duplicate_sec,
                                           const bool cover_sec_if_exist,
                                           const bool allow_duplicate_keys,
                                           const bool cover_key_if_exist)
{
  std::ifstream file_obj;
  file_obj.open(file_name, std::ios::in | std::ios::binary);

  if (!file_obj.is_open()) {
    auto err = inip::err::Errors(inip::err::ErrCode::CANNOT_OPEN_FILE, 0, this->file_name);
    err_list.push_back(err);
    return err;
  }

  // read from fstream to istringstream
  std::ostringstream oss;
  oss << file_obj.rdbuf();
  file_obj.close();

  std::istringstream iss(oss.str());

  return this->parse_str(iss,
                         err_list,
                         failed_skip,
                         use_import,
                         import_cover_sec_if_exist,
                         allow_duplicate_sec,
                         cover_sec_if_exist,
                         allow_duplicate_keys,
                         cover_key_if_exist);
}


inip::err::Errors inip::IniMgr::parse_file(std::vector<inip::err::Errors> &err_list,
                                           const bool failed_skip,
                                           const bool use_import,
                                           const bool import_cover_sec_if_exist,
                                           const bool allow_duplicate_sec,
                                           const bool cover_sec_if_exist,
                                           const bool allow_duplicate_keys,
                                           const bool cover_key_if_exist)
{

  return this->parse_file(this->file_name,
                          err_list,
                          failed_skip,
                          use_import,
                          import_cover_sec_if_exist,
                          allow_duplicate_sec,
                          cover_sec_if_exist,
                          allow_duplicate_keys,
                          cover_key_if_exist);
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

const std::map<std::string, inip::Section> *inip::IniMgr::get_data(void) const
{
  return &this->data;
}
