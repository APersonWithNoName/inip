#ifndef __COMMAND_HPP__
#define __COMMAND_HPP__

#include <vector>
#include <string>

#include "InipCore.hpp"

#define INIP_COMMAND_NEW_FUNCTION(fn_name) \
  int fn_name(int argc, char **argv, ::inip::demo::core::Environment &env); \
  static struct __AutoRegister_##fn_name { \
    __AutoRegister_##fn_name() { \
      ::inip::demo::core::cmd::get_cmdlist().push_back(#fn_name); \
    } \
  } __g_auto_reg_##fn_name;

#define INIP_COMMAND_DEFINE_FUNCTION(fn_name) \
  int ::inip::demo::core::cmd::fn_name(int argc, char **argv, ::inip::demo::core::Environment &env)

#define INIP_COMMAND_INIT(desc) \
  CLI::App app{#desc}

namespace inip
{
namespace demo
{
namespace core
{
namespace cmd
{

extern std::vector<std::string> g_cmdlist;

inline std::vector<std::string>& get_cmdlist() {
  static std::vector<std::string> list;
  return list;
}

INIP_COMMAND_NEW_FUNCTION(help)
INIP_COMMAND_NEW_FUNCTION(cat)
INIP_COMMAND_NEW_FUNCTION(valid)
INIP_COMMAND_NEW_FUNCTION(get)
INIP_COMMAND_NEW_FUNCTION(set)

INIP_COMMAND_NEW_FUNCTION(shell)

INIP_COMMAND_NEW_FUNCTION(setvar)
INIP_COMMAND_NEW_FUNCTION(unsetvar)

}
}
}
}

#endif
