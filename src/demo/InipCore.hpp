#ifndef __INIPCORE_HPP__
#define __INIPCORE_HPP__

#include "inip/ini.hpp"
#include <string>
#include <map>
#include <utility>


namespace inip {
namespace demo {
namespace core {

class CommandLine
{
public:
  std::string cmd;
  int argc;
  char **argv;
  bool valid;

  CommandLine();
  ~CommandLine();

  // 禁止拷贝
  CommandLine(const CommandLine &) = delete;
  CommandLine& operator=(const CommandLine &) = delete;
  // 允许移动
  CommandLine(CommandLine &&other) noexcept;

  void parse(const std::string &input);
};

enum class EnvironmentType
{
  SHELL,
  COMMAND,
};

class Environment{
public:
  EnvironmentType type;
  int latest_return_code = 0;
  std::map<std::string, std::string> var_list;
  std::map<std::string, ::inip::ini> ini_list;

  bool has_loaded(const std::string &file);
  ini& get_ini(const std::string &file);
  void add_ini(const std::string &file, const inip::ini &ini);
};

class CommandExec
{
public:
  Environment env;

  std::pair<bool, int> exec_one_cmd(const std::string &cmd);
  std::pair<bool, int> exec(const std::string &cmds);
};

}
}
}

#endif
