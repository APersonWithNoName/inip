#include <cstdlib>
#include <iostream>
#include <string>

#include "inip/inip.hpp"

#include "config.h"
#include "inip/Exception.hpp"
#include "inip/ini.hpp"
#include "version.h"

void print_help(const std::string &program)
{
  std::cout << "inip: simple ini parser." << std::endl;
  std::cout << "Usage:" << std::endl;
  std::cout << "    " << program << "<action> <file_name> [<options>]" << std::endl;
  std::cout << "Actions:        Options:                     Info:" << std::endl;
  std::cout << "    help                                      Print help info and exit." << std::endl;
  std::cout << "    version                                   Print version and exit.." << std::endl;
  
  std::cout << "    get         [<section>] <key> <def>       Get the value of the key," << std::endl;
  std::cout << "                                                return default_valuet if not exists." << std::endl;
  std::cout << "    set         [<section>] <key> <value>     Set the value of key." << std::endl;
}

int main(int argc, char **argv)
{
  std::string program = std::string(argv[0]);

  if (argc < 2) {
    print_help(program);
    return EXIT_FAILURE;
  }
  else {
    std::string action = std::string(argv[1]);
    if (argc == 2) {
      if (action == "help") {
        print_help(program);
        return EXIT_SUCCESS;
      }
    }
    else {
      std::string file_name = std::string(argv[2]);

      inip::ini ini(file_name);

#ifndef INIP_THROW_EXP_IF_ERROR
      try {
        ini.load_file();
      }
      catch (const inip::err::Errors &e) {
        std::cerr << "Error in load file. Line " << e.line << " code " << static_cast<int>(e.code) << std::endl;
        return EXIT_FAILURE;
      }
#else
      inip::err::Errors e = ini.load_file();
      if (e.code != inip::err::ErrCode::NO_ERRORS) {
        std::cerr << "Error in load file. Line " << e.line << " code " << static_cast<int>(e.code) << std::endl;
        return EXIT_FAILURE;
      }
#endif
      if (action == "get") {
        if (argc == 6) {
          std::cout << ini.get_default(std::string(argv[4]), std::string(argv[5]));
          return EXIT_SUCCESS;
        }
        else if (argc == 7) {
          std::cout << ini.get_default(std::string(argv[4]), std::string(argv[5]), std::string(argv[6]));
          return EXIT_SUCCESS;
        }
        else {
          print_help(program);
          return EXIT_FAILURE;
        }
      }
      else if (action == "set") {
        if (argc == 6) {
          ini.set(std::string(argv[4]), std::string(argv[5]));
          return EXIT_SUCCESS;
        }
        else if (argc == 7) {
          ini.set(std::string(argv[4]), std::string(argv[4]), std::string(argv[4]));
          return EXIT_SUCCESS;
        }
        else {
          print_help(program);
          return EXIT_FAILURE;
        }
      }
      else {
        print_help(program);
        return EXIT_FAILURE;
      }
    }
  }

  return EXIT_SUCCESS;
}
