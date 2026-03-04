// src/demo/cli/Shell.hpp
#ifndef __CLI_SHELL_HPP__
#define __CLI_SHELL_HPP__

#include <string>
#include <vector>
#include <map>
#include "replxx.hxx"
#include "InipCore.hpp"
#include "Configure.hpp"

namespace inip {
namespace demo {
namespace ui {

class Shell {
public:
  inip::demo::core::CommandExec cmdexec;

  Shell(const std::string& history_file = ".inip_history", 
        const std::string& prompt = "\x1b[1;32minip\x1b[0m> ");
  ~Shell();

  void configure(const ShellConfigure &sc);
  void init();
  int run();

private:
  void setupKeyBindings();
  void setupCallbacks();
  // 回调函数
  replxx::Replxx::completions_t onComplete(const std::string &context, int &contextLen);
  replxx::Replxx::hints_t onHint(const std::string &context, int &contextLen, 
                                  replxx::Replxx::Color color);
  void onHighlight(const std::string &line, replxx::Replxx::colors_t &colors);

  void highlightStrings(const std::string &line, replxx::Replxx::colors_t &colors);
  void highlightCommand(const std::string &line, replxx::Replxx::colors_t &colors);
  void highlightOptions(const std::string &line, replxx::Replxx::colors_t &colors);
  void highlightFiles(const std::string &line, replxx::Replxx::colors_t &colors);


  // 历史记录管理
  void load_history();
  void save_history();
  
  // 工具函数
  std::string read_line();
  void clear_screen();

  replxx::Replxx rx_;
  std::string history_file_;
  std::map<std::string, std::string> help_texts_;
  std::string prompt_;
  std::vector<std::string> commands_;

};

} // namespace ui
} // namespace demo
} // namespace inip

#endif
