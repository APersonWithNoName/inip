#ifndef __CLI_CONFIGURE_HPP__
#define __CLI_CONFIGURE_HPP__

#include <string>

namespace inip {
namespace demo {
namespace ui {

struct ShellConfigure {
  unsigned int max_history_size = 1000;
  unsigned int max_hint_rows = 5;
  unsigned int hint_delay = 500;
  unsigned int completion_count_off = 128;
  bool double_tab_completion = false;
  bool complete_on_empty = true;
  bool beep_on_ambiguous_completion = false;
  bool no_color = false;
  std::string word_break_characters = " \n\t\",%!;:=*~'/?<>|{}[]";
};

struct BatchConfigure {
  std::string command = "";
  std::string file = "";
};

} // namespace ui
} // namespace demo
} // namespace inip

#endif
