// src/demo/cli/Shell.cpp - 主文件
#include "Shell.hpp"
#include <fstream>
#include <iostream>
#include <functional>

#include "Command.hpp"
#include "inip/Tools.hpp"

namespace inip {
namespace demo {
namespace ui {

Shell::Shell(const std::string& history_file, const std::string& prompt) 
  : history_file_(history_file), prompt_(prompt) 
{
  commands_ = core::cmd::get_cmdlist();
}

Shell::~Shell() {
  save_history();
}

void Shell::init()
{
  this->cmdexec.env.type = inip::demo::core::EnvironmentType::SHELL;
  load_history();
}

void Shell::configure(const ShellConfigure &sc)
{
  // 基础配置
  rx_.set_max_history_size(sc.max_history_size);
  rx_.set_max_hint_rows(sc.max_hint_rows);
  rx_.set_hint_delay(sc.hint_delay);
  rx_.set_word_break_characters(sc.word_break_characters.c_str());
  rx_.set_completion_count_cutoff(sc.completion_count_off);
  rx_.set_double_tab_completion(sc.double_tab_completion);
  rx_.set_complete_on_empty(sc.complete_on_empty);
  rx_.set_beep_on_ambiguous_completion(sc.beep_on_ambiguous_completion);
  rx_.set_no_color(sc.no_color);

  // 设置回调
  setupCallbacks();
  
  // 设置快捷键
  setupKeyBindings();
}

void Shell::setupCallbacks()
{
  using namespace std::placeholders;
  
  rx_.set_completion_callback(
      std::bind(&Shell::onComplete, this, _1, _2));
  rx_.set_hint_callback(
      std::bind(&Shell::onHint, this, _1, _2, _3));
  rx_.set_highlighter_callback(
      std::bind(&Shell::onHighlight, this, _1, _2));
}

void Shell::setupKeyBindings()
{
  // Ctrl组合键
  rx_.bind_key_internal(replxx::Replxx::KEY::control('A'), "move_cursor_to_begining_of_line");
  rx_.bind_key_internal(replxx::Replxx::KEY::control('E'), "move_cursor_to_end_of_line");
  rx_.bind_key_internal(replxx::Replxx::KEY::control('L'), "clear_screen");
  rx_.bind_key_internal(replxx::Replxx::KEY::control('D'), "send_eof");
  rx_.bind_key_internal(replxx::Replxx::KEY::control('C'), "abort_line");
  rx_.bind_key_internal(replxx::Replxx::KEY::control('K'), "kill_to_end_of_line");
  rx_.bind_key_internal(replxx::Replxx::KEY::control('U'), "kill_to_begining_of_line");
  rx_.bind_key_internal(replxx::Replxx::KEY::control('W'), "kill_to_begining_of_word");
  rx_.bind_key_internal(replxx::Replxx::KEY::control('Y'), "yank");
  rx_.bind_key_internal(replxx::Replxx::KEY::control('T'), "transpose_characters");
  rx_.bind_key_internal(replxx::Replxx::KEY::control('R'), "history_incremental_search");

  // 光标移动
  rx_.bind_key_internal(replxx::Replxx::KEY::LEFT, "move_cursor_left");
  rx_.bind_key_internal(replxx::Replxx::KEY::RIGHT, "move_cursor_right");
  rx_.bind_key_internal(replxx::Replxx::KEY::UP, "line_previous");
  rx_.bind_key_internal(replxx::Replxx::KEY::DOWN, "line_next");
  rx_.bind_key_internal(replxx::Replxx::KEY::control(replxx::Replxx::KEY::LEFT), "move_cursor_one_word_left");
  rx_.bind_key_internal(replxx::Replxx::KEY::control(replxx::Replxx::KEY::RIGHT), "move_cursor_one_word_right");

  // 历史记录
  rx_.bind_key_internal(replxx::Replxx::KEY::meta(replxx::Replxx::KEY::UP), "history_previous");
  rx_.bind_key_internal(replxx::Replxx::KEY::meta(replxx::Replxx::KEY::DOWN), "history_next");
  rx_.bind_key_internal(replxx::Replxx::KEY::PAGE_UP, "history_first");
  rx_.bind_key_internal(replxx::Replxx::KEY::PAGE_DOWN, "history_last");

  // 其他
  rx_.bind_key_internal(replxx::Replxx::KEY::TAB, "complete_line");
  rx_.bind_key_internal(replxx::Replxx::KEY::control(replxx::Replxx::KEY::ENTER), "commit_line");
  rx_.bind_key_internal(replxx::Replxx::KEY::INSERT, "toggle_overwrite_mode");

#ifndef _WIN32
  rx_.bind_key_internal(replxx::Replxx::KEY::control('V'), "verbatim_insert");
  rx_.bind_key_internal(replxx::Replxx::KEY::control('Z'), "suspend");
#endif

  // Meta组合键
  rx_.bind_key_internal(replxx::Replxx::KEY::meta(replxx::Replxx::KEY::BACKSPACE), "kill_to_whitespace_on_left");
  rx_.bind_key_internal(replxx::Replxx::KEY::meta('p'), "history_common_prefix_search");
  rx_.bind_key_internal(replxx::Replxx::KEY::meta('n'), "history_common_prefix_search");
  rx_.bind_key_internal(replxx::Replxx::KEY::meta('d'), "kill_to_end_of_word");
  rx_.bind_key_internal(replxx::Replxx::KEY::meta('y'), "yank_cycle");
  rx_.bind_key_internal(replxx::Replxx::KEY::meta('u'), "uppercase_word");
  rx_.bind_key_internal(replxx::Replxx::KEY::meta('l'), "lowercase_word");
  rx_.bind_key_internal(replxx::Replxx::KEY::meta('c'), "capitalize_word");
}

replxx::Replxx::completions_t Shell::onComplete(const std::string &context, int &contextLen)
{
  replxx::Replxx::completions_t completions;
  contextLen = static_cast<int>(context.size());

  for (const auto &cmd : this->commands_) {
    if (cmd.find(context) == 0) {
      completions.push_back(replxx::Replxx::Completion(cmd, replxx::Replxx::Color::DEFAULT));
    }
  }
  return completions;
}

replxx::Replxx::hints_t Shell::onHint(const std::string &context, int &contextLen, replxx::Replxx::Color color)
{
  replxx::Replxx::hints_t hints;
  contextLen = static_cast<int>(context.size());
  return hints;
}

void Shell::onHighlight(const std::string &line, replxx::Replxx::colors_t &colors)
{
  if (colors.size() < line.size()) {
    colors.resize(line.size());
  }

  // 字符串高亮
  highlightStrings(line, colors);
  
  // 命令名高亮
  highlightCommand(line, colors);
  
  // 选项高亮
  highlightOptions(line, colors);
  
  // 文件高亮
  highlightFiles(line, colors);
}

// Shell.cpp 中添加这个函数
void Shell::highlightStrings(const std::string &line, replxx::Replxx::colors_t &colors)
{
  bool in_string = false;
  char string_quote = '\0';

  for (size_t i = 0; i < line.length(); ++i) {
    char c = line[i];

    // 处理转义字符
    if (c == '\\' && in_string) {
      if (i + 1 < line.length()) {
        colors[i] = replxx::Replxx::Color::YELLOW;
        colors[i + 1] = replxx::Replxx::Color::YELLOW;
        i++;
      }
      continue;
    }

    // 处理字符串引号
    if (c == '"' || c == '\'') {
      if (!in_string) {
        in_string = true;
        string_quote = c;
        colors[i] = replxx::Replxx::Color::YELLOW; // 引号颜色
      }
      else if (c == string_quote) {
        in_string = false;
        colors[i] = replxx::Replxx::Color::YELLOW; // 引号颜色
      }
      continue;
    }

    // 在字符串内部，使用不同颜色
    if (in_string) {
      colors[i] = replxx::Replxx::Color::YELLOW;
    }
  }
}

void Shell::highlightCommand(const std::string &line, replxx::Replxx::colors_t &colors)
{
  std::string::size_type space_pos = line.find(' ');
  std::string cmd = line.substr(0, space_pos);

  // 命令名高亮
  for (const auto &c : this->commands_) {
    if (cmd == c) {
      for (std::string::size_type i = 0; i < cmd.size(); i++) {
        colors[i] = replxx::Replxx::Color::BRIGHTGREEN;
      }
      break;
    }
  }
}

void Shell::highlightOptions(const std::string &line, replxx::Replxx::colors_t &colors)
{
  // --flag 高亮
  std::string::size_type pos = 0;
  while ((pos = line.find("--", pos)) != std::string::npos) {
    bool in_string_region = false;

    for (std::string::size_type j = 0; j < pos; j++) {
      if (line[j] == '"' || line[j] == '\'') {
        in_string_region = !in_string_region;
      }
    }

    if (!in_string_region) {
      std::string::size_type start_pos = pos;
      pos += 2;

      std::vector<char> delimiters = {' ', '\t', '"', '\''};

      while (pos < line.length() &&
             !inip::Tools::vector_equal_one(line[pos], delimiters)) {
        pos++;
      }

      for (std::string::size_type i = start_pos; i < pos; i++) {
        colors[i] = replxx::Replxx::Color::GRAY;
      }
    }
    else {
      pos += 2;
    }
  }

  // = 高亮
  pos = 0;
  while ((pos = line.find('=', pos)) != std::string::npos) {
    bool in_string_region = false;
    for (size_t j = 0; j < pos; ++j) {
      if (line[j] == '"' || line[j] == '\'') {
        in_string_region = !in_string_region;
      }
    }

    if (!in_string_region) {
      colors[pos] = replxx::Replxx::Color::CYAN;
    }
    pos++;
  }

  // -f 短选项高亮
  pos = 0;
  while ((pos = line.find(" -", pos)) != std::string::npos) {
    bool in_string_region = false;
    for (size_t j = 0; j < pos; ++j) {
      if (line[j] == '"' || line[j] == '\'') {
        in_string_region = !in_string_region;
      }
    }

    if (!in_string_region) {
      pos++; // 移动到 '-'
      if (pos + 1 < line.length() && line[pos + 1] != '-') {
        size_t start = pos;

        if (pos < line.length()) {
          colors[start] = replxx::Replxx::Color::GRAY;
          if (pos + 1 < line.length() &&
              line[pos + 1] != ' ' && line[pos + 1] != '\t') {
            colors[start + 1] = replxx::Replxx::Color::GRAY;
          }
        }
      }
    }
    pos++;
  }
}

void Shell::highlightFiles(const std::string &line, replxx::Replxx::colors_t &colors)
{
  // 高亮 .ini 文件名
  size_t ini_pos = 0;
  while ((ini_pos = line.find(".ini", ini_pos)) != std::string::npos) {
    // 检查是否在字符串内
    bool in_string_region = false;
    for (size_t j = 0; j < ini_pos; ++j) {
      if (line[j] == '"' || line[j] == '\'') {
        in_string_region = !in_string_region;
      }
    }

    if (!in_string_region) {
      // 找到文件名开始
      size_t start = ini_pos;
      while (start > 0 && line[start - 1] != ' ' &&
             line[start - 1] != '\t' && line[start - 1] != '"' &&
             line[start - 1] != '\'') {
        start--;
      }

      // 高亮整个文件名
      size_t end = ini_pos + 4;
      for (size_t i = start; i < end && i < colors.size(); ++i) {
        colors[i] = replxx::Replxx::Color::YELLOW;
      }
    }
    ini_pos += 4;
  }
}

void Shell::load_history()
{
  if (!this->history_file_.empty()){
    std::ifstream history_file(history_file_.c_str());
    if (history_file.is_open()) {
      rx_.history_load(history_file);
    }
    else {
      std::cerr << "Cannot open history file: " << this->history_file_ << std::endl;
    }
  }
}

void Shell::save_history()
{
  if (!this->history_file_.empty()) {
    rx_.history_sync(this->history_file_);
  }
}

std::string Shell::read_line()
{
  const char* line = rx_.input(this->prompt_);
  if (line == nullptr) return "";
  return std::string(line);
}

int Shell::run()
{
  int ret_code = 0;

  DEBUG("Shell start.");

  while(true) {
    std::string line = read_line();
    DEBUG("Line: " << line)

    if (line.empty()) {
      continue;
    }

    // 添加到历史记录
    if (line != "exit" && line != "quit") {
      rx_.history_add(line);
    }

    DEBUG("Exec: " << line)
    std::pair<bool, int> result = this->cmdexec.exec(line);
    ret_code = result.second;
    DEBUG("Result: " << result.first << "," << result.second)

    if (result.first || line == "exit" || line == "quit") {
      DEBUG("Require exit.")
      break;
    }
  }

  DEBUG("Shell exit with code " << ret_code)
  return ret_code;
}

} // namespace ui
} // namespace demo
} // namespace inip
