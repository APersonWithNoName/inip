#include <cctype>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>

#include "inip/Tools.hpp"

std::vector<std::string> inip::Tools::split(const std::string &str, const char delim)
{
  std::vector<std::string> result;
  if (str.empty()) {
    return result;
  }

  std::stringstream ss(str);
  std::string token;

  while (std::getline(ss, token, delim)) {
    result.push_back(token);
  }

  // std::getline 不会为结尾的分隔符添加空字符串
  // 如果需要保持一致的行为,需要特殊处理
  if (!str.empty() && str.back() == delim) {
    result.push_back("");
  }

  return result;
}

std::vector<std::string> inip::Tools::split(const std::string &str, const std::string &delim, const bool skip_empty)
{
  std::vector<std::string> result;

  // 如果分隔符为空,将每个字符作为一个元素
  if (delim.empty()) {
    for (const char ch : str) {
      result.push_back(std::string(1, ch));
    }
    return result;
  }

  if (str.empty()) {
    return result;
  }

  size_t pos = 0, prev = 0;

  while ((pos = str.find(delim, prev)) != std::string::npos) {
    std::string token = str.substr(prev, pos - prev);
    if (!skip_empty || !token.empty()) {
      result.push_back(token);
    }
    prev = pos + delim.length();
  }

  std::string lastToken = str.substr(prev);
  if (!skip_empty || !lastToken.empty()) {
    result.push_back(lastToken);
  }

  return result;
}

std::string inip::Tools::trim(const std::string &istr)
{
  auto front = std::find_if_not(istr.begin(), istr.end(), [](unsigned char c){
    return std::isspace(c);
  });
  auto back = std::find_if_not(istr.rbegin(), istr.rend(), [](unsigned char c){
    return std::isspace(c);
  }).base();

  if (front >= back) {
    return "";
  }

  return std::string(front, back);
}

inip::Tools::IniPath inip::Tools::parse_ini_path(const std::string &str)
{
  inip::Tools::IniPath ini_path;
  std::string trimmed = inip::Tools::trim(str);

  if (trimmed.empty()) return ini_path;

  // 查找冒号分隔符
  std::string::size_type colon = trimmed.find(':');
  if (colon == std::string::npos) return ini_path;

  // 解析文件名
  ini_path.file = trimmed.substr(0, colon);
  if (ini_path.file.empty()) return ini_path;

  // 解析路径部分
  std::string path = trimmed.substr(colon + 1);
  if (path.empty()) return ini_path;

  // 处理 /section/key 或 /key 格式
  if (path[0] == '/') {
    path = path.substr(1);
  }

  std::string::size_type slash = path.find('/');
  if (slash == std::string::npos) {
    ini_path.key = path;
    ini_path.section = "";
  }
  else {
    ini_path.section = path.substr(0, slash);
    ini_path.key = path.substr(slash + 1);
  }

  if (ini_path.key.empty()) return ini_path;

  ini_path.valid = true;
  return ini_path;
}

inip::Tools::VarExpr inip::Tools::parse_varexpr(const std::string &var_str)
{
  inip::Tools::VarExpr result;

  if (var_str.size() < 3 || var_str.front() != '$' || var_str.back() != '$') {
    return result;
  }
  // 提取中间内容（去掉首尾的 $）
  std::string inner = var_str.substr(1, var_str.size() - 2);

  std::string content;
  bool escape = false;

  for (const char c : inner) {
    if (escape) {
      content += c;
      escape = false;
      continue;
    }

    if (c == '\\') {
      escape = true;
      continue;
    }

    content += c;
  }

  std::string name, def;
  escape = false;
  bool in_def = false;

  for (const char c : content) {
    if (escape) {
      if (in_def) def += c;
      else name += c;
      escape = false;
      continue;
    }

    if (c == '\\') {
      escape = true;
      continue;
    }

    if (c == '|' && !in_def) {
      in_def = true;
      continue;;
    }

    if (in_def) def += c;
    else name += c;
  }

  result.name = name;
  if (in_def) {
    result.has_def = true;
    result.def = def;
  }

  return result;
}

std::string inip::Tools::replace_str(const std::string &str,
  const std::map<std::string, std::string> varlist,
  const std::function<VarExprDefaultFn(const VarExpr varexpr)> default_value)
{
  std::string result;
  std::string::size_type i = 0;
  bool escape = false;

  while (i < str.size()) {
    char c = str[i];
    
    // 处理转义
    if (escape) {
      result += c;
      escape = false;
      i++;
      continue;
    }

    if (c == '\\') {
      escape = true;
      i++;
      continue;
    }

    // 处理变量 ${...}
    if (c == '$') {
      std::string::size_type j = i + 1;
      bool inner_escape = false;

      // 查找匹配的结束 $
      while (j < str.size()) {
        if (inner_escape) {
          inner_escape = false;
          j++;
          continue;
        }
        
        if (str[j] == '\\') {
          inner_escape = true;
          j++;
          continue;
        }
        
        if (str[j] == '$') {
          break;
        }
        j++;
      }

      // 找到完整的变量表达式 $...$
      if (j < str.size() && str[j] == '$') {
        std::string var_expr = str.substr(i, j - i + 1);
        auto var = parse_varexpr(var_expr);
        
        if (!var.name.empty()) {
          auto it = varlist.find(var.name);
          if (it != varlist.end()) {
            result += it->second;  // 使用变量值
          } else {
            if (var.has_def) {
              result += var.def;   // 使用默认值
            } else {
              auto def_value = default_value(var);
              if (def_value.abort != 0) {
                if (def_value.abort < 0) {
                  std::cerr << def_value.msg << std::endl;
                }
                // 遇到错误，返回已处理的部分
                return result;
              } else {
                result += def_value.value;
              }
            }
          }
          // 跳过整个变量表达式
          i = j + 1;
          continue;
        }
      }
    }
    
    // 普通字符直接添加
    result += c;
    i++;
  }

  return result;
}


inip::Tools::VarExprDefaultFn inip::Tools::varexpr_default(const VarExpr varexpr)
{
  inip::Tools::VarExprDefaultFn default_fn;
  default_fn.abort = 0;
  default_fn.msg = "";
  default_fn.value = "";
  return default_fn;
}
