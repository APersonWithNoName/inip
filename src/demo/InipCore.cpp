#include <cctype>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "Command.hpp"
#include "InipCore.hpp"
#include "inip/Tools.hpp"


inip::demo::core::CommandLine::CommandLine() : argc(0), argv(nullptr), valid(false) {}

inip::demo::core::CommandLine::~CommandLine()
{
  if (this->argv) {
    for (size_t i = 0; i < argc; i++) {
      delete [] argv[i];
    }
    delete [] argv;
  }
}

inip::demo::core::CommandLine::CommandLine(CommandLine &&other) noexcept : cmd(std::move(other.cmd)), argc(other.argc), argv(other.argv)
{
  other.argc = 0;
  other.argv = nullptr;
}

void inip::demo::core::CommandLine::parse(const std::string &input)
{
  std::vector<std::string> tokens;
  std::string current;
  bool escape = false;
  bool in_quotes = false;
  char quote_char = '\0';

  for (std::string::size_type i = 0; i < input.size(); i++) {
    char c = input[i];

    // 处理转义字符
    if (escape) {
      current += c;
      escape = false;
      continue;
    }

    // 处理转义符号
    if (c == '\\') {
      escape = true;
      continue;
    }

    // 处理 @ 符号（你的特殊用途）
    if (c == '@') {
      continue;
    }

    // 处理引号
    if (c == '"' || c == '\'') {
      if (!in_quotes) {
        // 进入引号模式
        in_quotes = true;
        quote_char = c;
      } else if (c == quote_char) {
        // 退出引号模式（只有匹配的引号才退出）
        in_quotes = false;
      } else {
        // 不同类型的引号，当作普通字符处理
        current += c;
      }
      continue;
    }

    // 只有在非引号模式下才将空白字符作为分隔符
    if (!in_quotes && (std::isspace(c) || c == ',')) {
      if (!current.empty()) {
        tokens.push_back(current);
        current.clear();
      }
      continue;
    }

    // 普通字符，直接添加
    current += c;
  }

  // 处理最后一个 token
  if (!current.empty()) {
    tokens.push_back(current);
  }

  // 如果没有解析到任何 token，直接返回
  if (tokens.empty()) {
    this->valid = false;
    return;
  }

  // 清理旧的 argv（如果有）
  if (this->argv) {
    for (size_t i = 0; i < this->argc; i++) {
      delete[] this->argv[i];
    }
    delete[] this->argv;
    this->argv = nullptr;
  }

  // 设置命令和参数
  this->cmd = tokens[0];
  this->argc = tokens.size();
  this->argv = new char*[this->argc + 1];

  for (size_t i = 0; i < this->argc; i++) {
    this->argv[i] = new char[tokens[i].size() + 1];
    std::strcpy(this->argv[i], tokens[i].c_str());
  }

  this->argv[this->argc] = nullptr;
  this->valid = true;
}

bool inip::demo::core::Environment::has_loaded(const std::string &file)
{
  return (this->ini_list.find(file) != this->ini_list.end()); 
}

inip::ini& inip::demo::core::Environment::get_ini(const std::string &file)
{
  return this->ini_list[file];
}

void inip::demo::core::Environment::add_ini(const std::string &file, const inip::ini &ini)
{
  this->ini_list[file] = ini;
}

std::pair<bool, int>  inip::demo::core::CommandExec::exec(const std::string &cmds)
{
  DEBUG("Receive commands: " << cmds)
  std::vector<std::string> cmd_list = {};
  std::pair<bool, int>  ret_code(false, 0);

  DEBUG("--- Before replace_str")
  cmd_list = inip::Tools::split(inip::Tools::replace_str(cmds, this->env.var_list,
    [](const inip::Tools::VarExpr varexpr) -> inip::Tools::VarExprDefaultFn {
      inip::Tools::VarExprDefaultFn default_fn;

      std::cerr << "Unknown Var: " << varexpr.name << std::endl;
      std::cerr << "Use empty as default" << std::endl;

      default_fn.abort = 0;
      default_fn.msg = "Unknown var.";
      default_fn.value = "";

      return default_fn;
    }
  ), ';');
  DEBUG("--- After replace_str")

  DEBUG("Exec cmd vector");

#ifdef DEBUG__
  for (size_t i = 0; i < cmd_list.size(); i++) {
    DEBUG("Cmd["<<i<<"], should exec:"<<cmd_list[i])
  }
#endif
  for (const auto &cmd : cmd_list) {
    DEBUG("Send commands to exec_one_cmd: " << cmd)
    ret_code = exec_one_cmd(cmd);
  }

  this->env.latest_return_code = ret_code.second;
  return ret_code;
}

#define INIP_COMMAND_CALL(cmd_name) \
  else if (cmdline.cmd == #cmd_name) result.second = ::inip::demo::core::cmd::cmd_name(cmdline.argc, cmdline.argv, this->env)

std::pair<bool, int>  inip::demo::core::CommandExec::exec_one_cmd(const std::string &cmd)
{
  DEBUG("Receive command: " << cmd);
  CommandLine cmdline;
  cmdline.parse(cmd);
  std::pair<bool, int> result(false, 0);

  if (cmdline.cmd == "quit" || cmdline.cmd == "exit") {
    DEBUG("Receive quit, exit.")
    return std::pair<bool, int>(true, 0);
  }
  else if (cmdline.cmd == "echo") {
    for (unsigned int i = 1; i < cmdline.argc; i++) {
      if (i > 1) std::cout << " ";
      std::cout << cmdline.argv[i];
    }
    std::cout << std::endl;
    result.second = 0;
  }
  INIP_COMMAND_CALL(help);
  INIP_COMMAND_CALL(set);
  INIP_COMMAND_CALL(get);
  INIP_COMMAND_CALL(valid);
  INIP_COMMAND_CALL(cat);
  INIP_COMMAND_CALL(shell);
  INIP_COMMAND_CALL(setvar);
  INIP_COMMAND_CALL(unsetvar);
  else {
    DEBUG("Unknown command:" << cmdline.cmd)
    std::cerr << "Unknown command: " << cmdline.cmd << std::endl;
    return result;
  }

  DEBUG("Result: " << result.first << "," << result.second)
  return result;
}
