#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "InipCore.hpp"
#include "Command.hpp"
#include "inip/IniMgr.hpp"
#include "inip/ini.hpp"
#include "inip/Tools.hpp"
#include "inip/Exception.hpp"

#include "CLI/CLI.hpp"
#include "CLI/App.hpp"

namespace {

// 加载INI文件，如果已加载则从环境获取
inip::ini load_ini_file(inip::demo::core::Environment &env, 
                        const std::string &file_path,
                        std::vector<inip::err::Errors> &err_list) {
  if (env.has_loaded(file_path)) {
    return env.get_ini(file_path);
  }
  
  inip::ini ini(file_path);
  inip::err::Errors err = ini.load_file(err_list, false);
  
  if (err.get_code_err() == inip::err::ErrCode::NO_ERRORS) {
    env.add_ini(file_path, ini);
  }
  
  return ini;
}

// 打印错误列表
void print_errors(const std::vector<inip::err::Errors> &err_list) {
  if (err_list.empty()) return;
  
  std::cerr << "Found " << err_list.size() << " error(s):" << std::endl;
  for (const auto &err : err_list) {
    std::cerr << "  File: " << err.file_name 
              << ", line: " << err.line 
              << ", code: " << err.get_code() << std::endl;
  }
}

// 解析IniPath并验证
inip::Tools::IniPath parse_and_validate_ini_path(const std::string &ini_path_str) {
  auto ini_path = inip::Tools::parse_ini_path(ini_path_str);
  if (!ini_path.valid) {
    std::cerr << "Error: Invalid IniPath format: " << ini_path_str << std::endl;
  }
  return ini_path;
}

} // anonymous namespace


INIP_COMMAND_DEFINE_FUNCTION(help)
{
  INIP_COMMAND_INIT("Show help information.");

  std::string cmd;
  app.add_option("command", cmd, "Specific command to show help for");

  CLI11_PARSE(app, argc, argv);

  if (!cmd.empty()) {
    // 显示特定命令的帮助
    std::stringstream ss;
    ss << cmd << " --help";
    // TODO: 执行命令的 --help
    std::cout << "Help for command '" << cmd << "'" << std::endl;
  } else {
    // 显示所有可用命令
    std::cout << "\nAvailable commands:" << std::endl;
    std::cout << "  help   - Show this help message" << std::endl;
    std::cout << "  cat    - Display file contents" << std::endl;
    std::cout << "  valid  - Validate INI file format" << std::endl;
    std::cout << "  get    - Get value from INI file" << std::endl;
    std::cout << "  set    - Set value in INI file" << std::endl;
    std::cout << "\nUse 'help <command>' for specific command help." << std::endl;
  }

  return 0;
}


INIP_COMMAND_DEFINE_FUNCTION(cat)
{
  INIP_COMMAND_INIT("Display contents of a file.");

  std::string file;
  bool show_line_numbers = false;
  bool show_nonprinting = false;

  app.add_option("file", file, "File to display")->required();
  app.add_flag("-n,--number", show_line_numbers, "Show line numbers");
  app.add_flag("-v,--show-nonprinting", show_nonprinting, "Show non-printing characters");

  CLI11_PARSE(app, argc, argv);

  std::ifstream f(file);
  if (!f.is_open()) {
    std::cerr << "Error: Cannot open file: " << file << std::endl;
    return -1;
  }

  std::string line;
  int line_num = 1;

  while (std::getline(f, line)) {
    if (show_line_numbers) {
      std::cout << std::setw(6) << line_num << "  ";
    }

    if (show_nonprinting) {
      for (char c : line) {
        if (std::isprint(static_cast<unsigned char>(c))) {
          std::cout << c;
        } else {
          std::cout << '\\' << std::oct << static_cast<int>(c);
        }
      }
    } else {
      std::cout << line;
    }
    
    std::cout << std::endl;
    line_num++;
  }

  if (f.bad()) {
    std::cerr << "Error: Failed to read file: " << file << std::endl;
    return -1;
  }

  return 0;
}


INIP_COMMAND_DEFINE_FUNCTION(valid)
{
  INIP_COMMAND_INIT("Validate INI file format.");

  std::string file;
  bool verbose = false;

  app.add_option("file", file, "INI file to validate")->required();
  app.add_flag("-v,--verbose", verbose, "Show detailed error information");

  CLI11_PARSE(app, argc, argv);

  std::vector<inip::err::Errors> err_list;
  inip::ini ini(file);
  inip::err::Errors result = ini.load_file(err_list, false);

  if (err_list.empty() && result.get_code_err() == inip::err::ErrCode::NO_ERRORS) {
    std::cout << "Valid: " << file << " is a valid INI file" << std::endl;
    return 0;
  }

  std::cout << "Invalid: " << file << " contains errors" << std::endl;
  
  if (verbose) {
    print_errors(err_list);
    if (result.get_code_err() != inip::err::ErrCode::NO_ERRORS) {
      std::cerr << "  Load error: code " << result.get_code() << std::endl;
    }
  } else {
    std::cout << "Total errors: " << err_list.size() << std::endl;
    std::cout << "Use -v for detailed error information" << std::endl;
  }

  return -1;
}


INIP_COMMAND_DEFINE_FUNCTION(get)
{
  
  INIP_COMMAND_INIT("Get value from INI file.");

  std::string ini_path_str;
  std::string default_value;
  bool show_all = false;

  app.add_option("path", ini_path_str, "INI path in format: file:[/section/]key")->required();
  app.add_option("-d,--default", default_value, "Default value if key not found");
  app.add_flag("-a,--all", show_all, "Show all values in section");

  CLI11_PARSE(app, argc, argv);

  // 解析路径
  auto ini_path = parse_and_validate_ini_path(ini_path_str);
  if (!ini_path.valid) {
    return -1;
  }

  // 加载INI文件
  std::vector<inip::err::Errors> err_list;
  inip::ini ini = load_ini_file(env, ini_path.file, err_list);

  if (!err_list.empty()) {
    print_errors(err_list);
    return -1;
  }

  try {
    if (show_all && !ini_path.section.empty()) {
      // 显示整个section
      auto keys = ini.get_section(ini_path.section);
      for (const auto &key : keys.keys()) {
        std::cout << key << " = " << ini.get(ini_path.section, key) << std::endl;
      }
    } else if (!default_value.empty()) {
      // 使用默认值
      std::cout << ini.get_default(ini_path.section, ini_path.key, default_value) << std::endl;
    } else {
      // 正常获取
      std::cout << ini.get(ini_path.section, ini_path.key) << std::endl;
    }
  } catch (const std::exception &e) {
    if (default_value.empty()) {
      std::cerr << "Error: " << e.what() << std::endl;
      return -1;
    }
    // 有默认值时不报错
    std::cout << default_value << std::endl;
  }

  return 0;
}


INIP_COMMAND_DEFINE_FUNCTION(set)
{
  INIP_COMMAND_INIT("Set value in INI file.");

  std::string ini_path_str;
  std::string value;
  bool dry_run = false;
  bool create_backup = false;

  app.add_option("path", ini_path_str, "INI path in format: file:[/section/]key")->required();
  app.add_option("value", value, "Value to set")->required();
  app.add_flag("-n,--dry-run", dry_run, "Show what would be done without writing");
  app.add_flag("-b,--backup", create_backup, "Create backup file before writing");

  CLI11_PARSE(app, argc, argv);

  // 解析路径
  auto ini_path = parse_and_validate_ini_path(ini_path_str);
  if (!ini_path.valid) {
    return -1;
  }

  // 加载INI文件
  std::vector<inip::err::Errors> err_list;
  inip::ini ini = load_ini_file(env, ini_path.file, err_list);

  if (!err_list.empty()) {
    print_errors(err_list);
    return -1;
  }

  if (dry_run) {
    std::cout << "Would set: " << ini_path_str << " = " << value << std::endl;
    return 0;
  }

  // 创建备份
  if (create_backup) {
    std::string backup_file = ini_path.file + ".bak";
    std::ifstream src(ini_path.file, std::ios::binary);
    std::ofstream dst(backup_file, std::ios::binary);
    dst << src.rdbuf();
    std::cout << "Backup created: " << backup_file << std::endl;
  }

  // 设置值并保存
  ini.set(ini_path.section, ini_path.key, value);
  ini.write();

  std::cout << "Set: " << ini_path_str << " = " << value << std::endl;
  return 0;
}


INIP_COMMAND_DEFINE_FUNCTION(shell)
{
  INIP_COMMAND_INIT("Executing shell commands.");

  std::string command;
  app.add_option("command", command, "The command to execute.");

  CLI11_PARSE(app, argc, argv);

  return system(command.c_str());
}


INIP_COMMAND_DEFINE_FUNCTION(setvar)
{
  INIP_COMMAND_INIT("Create var");

  std::string name;
  app.add_option("name", name, "The name of the var.")->required();
  std::string value;
  app.add_option("value", value, "The value of var.");

  CLI11_PARSE(app, argc, argv);

  if (name.empty()) {
    std::cerr << "Var name cannot be empty." << std::endl;
    return -1;
  }

  env.var_list[name] = value;

  return 0;
}


INIP_COMMAND_DEFINE_FUNCTION(unsetvar)
{
  INIP_COMMAND_INIT("Unset var");

  std::string name;
  app.add_option("name", name, "The name of var");

  CLI11_PARSE(app, argc, argv);

  if (env.var_list.find(name) != env.var_list.end()) {
    env.var_list.erase(name);
  }

  return 0;
}
