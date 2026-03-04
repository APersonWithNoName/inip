#include <string>

#include "CLI/App.hpp"
#include "CLI/CLI.hpp"

#include "Batch.hpp"
#include "Shell.hpp"
#include "Configure.hpp"

int main(int argc, char **argv)
{
  CLI::App app{"Inip demo", "inip"};

  std::string command;
  std::string filename;
  bool shell_mode = true;

  app.allow_non_standard_option_names();

  app.add_option("-c,--command", command, "The command.");
  app.add_option("-f,--file", filename, "The script file.");
  app.add_flag("--shell", shell_mode, "Use shell.");

  CLI11_PARSE(app, argc, argv);

  // 确定运行模式
  if (!command.empty() || !filename.empty()) {
    shell_mode = false;
  }

  if (shell_mode) {
    inip::demo::ui::Shell shellapp;
    inip::demo::ui::ShellConfigure sc;
    
    shellapp.configure(sc);
    shellapp.init();
    return shellapp.run();
  }
  else {
    inip::demo::ui::Batch batchapp;
    inip::demo::ui::BatchConfigure bc;

    bc.command = command;
    bc.file = filename;

    batchapp.configure(bc);
    batchapp.init();
    return batchapp.run();
  }

  return 0;
}
