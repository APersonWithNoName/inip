/**
 * @file inip_advance.cpp
 * @brief inip Advanced functions
 * @author APersonWithNoName
 * @version 0.1.0
 * @date 2026-08-20
 */
#include "inip/inip.hpp"

#include <cstdlib>


int main(int argc, char **argv)
{
  inip::err::ErrList errlist;

  const std::string file_name = "inip_base.ini";
  const std::string ini_content =
    "key = value\n"
    "[db]\n"
    "port = 22\n"
    "ip = localhost\n"
    "[user1]\n"
    "user = user1\n"
    "[user2]\n"
    "user = user2\n"
    "\n";

  inip::ini ini(file_name);

  /// Specify another file
  auto file_result = ini.load_file("inip_advance.ini", errlist, true);
  /// Load from string
  auto str_result = ini.load_str(ini_content, errlist);

  /// Global sections
  /// Use "" as name, or use a override
  ini.set("key", "value");
  ini.set("", "key", "value"); ///< Equivalent
  ini.get("key");
  ini.get("", "key");

  /// NOTE: Specific type functions rquuires ""
  ini.set_bool("", "boolkey", false);

  /// Use template functions `const std::function<std::string(const T def_value)>` as convertor
  ini.set_value<std::string>("section", "key", "string", [](const std::string &s) { return s; });
  /// `const std::function<T(const std::string raw_str)>` as convertor
  ini.get_value<std::string>("section", "key", [](const std::string &s) { return s; });

  /// Get name of all sections
  /// Use`std::vector<std::string>`default or `std::list<std::string>>`
  auto all_sections = ini.section_names();

  /// Get all sections' const reference
  /// Use `std::vector<std::reference_wrapper<const ::inip::Section>>`(drfault)
  /// or `std::list<std::reference_wrapper<const ::inip::Section>>`
  auto all_sections_ref = ini.sections();

  return EXIT_SUCCESS;
}
