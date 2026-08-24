#include <iostream>
#include <string>

#include "CLI/CLI.hpp"
#include "CLI/Error.hpp"
#include "inip/inip.hpp"

#include "cliconfig.h"


struct ArgRuntime{
  bool version = false;
  bool failed_skip = true;
  bool quiet = false;
  std::string filename;
  std::string secname;
  std::string key;
  std::string value;
  bool m_valid = false;
  bool m_clean = false;
  bool m_get = false;
};

namespace {
void print_inip_errors(const inip::err::ErrList &e)
{
  for (const auto &i : e) {
    std::cerr
      << "file: " << i.get_file() << "\n"
      << "line: " << i.get_line() << "\n"
      << "code: " << i.get_code() << "\n"
      << "info: " << i.to_string() << "\n";
  }
}
}


int main(int argc, char **argv)
{
  ArgRuntime argrt;

  CLI::App app("inip CLI prigram - A tiny tiny INI CLI parser");

  app.add_flag("--version", argrt.version, "Print version and exit");
  app.add_flag("-F,--failed_skip", argrt.failed_skip, "Whether skip failure and continue or stop at first");
  app.add_option("-f,--file", argrt.filename, "The file name, if not specfied, use stdin");
  app.add_option("-s,--section", argrt.secname, "The section name");
  app.add_option("-k,--key", argrt.key, "The key");
  app.add_option("-v,--value", argrt.value, "The value");
  app.add_flag("-V,--valid", argrt.m_valid, "Check whether the file valid, return code is the count of errors");
  app.add_flag("-C,--clean", argrt.m_clean, "Check errors and print file without comments");
  app.add_flag("-G,--get", argrt.m_get, "Print the value of");
  app.add_flag("-Q,--quiet", argrt.quiet, "Quiet mode");

  try {
    app.parse(argc, argv);
  }
  catch (const CLI::ParseError &e) {
    std::cerr << "ParseError: " << e.get_name() << "\n";
    app.exit(e);
  }

  if (argrt.version) {
    std::cout << "inip version " << PROJECT_VERSION << "\n";
    return 0;
  }

  inip::err::ErrList errlist;
  inip::ini inifile("");
  inip::err::Errors result;
  if (argrt.filename.empty()) {
    std::string line, str;
    while (std::getline(std::cin, line)) {
      str += line;
      str += "\n";
    }
    result = inifile.load_str(str, errlist, argrt.failed_skip);
  }
  else {
    result = inifile.load_file(argrt.filename, errlist, argrt.failed_skip);
  }

  auto err_cnt = static_cast<int>(errlist.size());

  if (!result.valid() && !argrt.quiet) {
    print_inip_errors(errlist);
  }

  if (argrt.m_valid) {
    if (!argrt.quiet && !result.valid() && errlist.size() > 0) {
      std::cerr << "Total errors: " << errlist.size() << "\n";
    }

    if (result.valid()) {
      std::cout << "valid\n";
    }
    else {
      std::cout << "invalid\n";
    }

    return err_cnt;
  }

  if (argrt.m_clean) {
    std::cout << inifile.to_string() << "\n";
    return err_cnt;
  }

  if (argrt.m_get) {
    if (argrt.value.empty()) {
      try {
        std::cout << inifile.get(argrt.secname, argrt.key) << "\n";
        return err_cnt;
      }
      catch (const inip::err::Errors &e) {
        std::cerr << "Cannot get key '" << argrt.key << "': " << e.to_string() << "\n";
        return err_cnt;
      }
    }
    else {
      std::cout << inifile.get_default(argrt.secname, argrt.key, argrt.value) << "\n";
      return err_cnt;
    }
  }

  return 0;
}
