/**
 * @file inip_base.cpp
 * @brief inip Basic functions
 * @author APersonWithNoName
 * @version 0.1.0
 * @date 2026-08-20
 */

/// Equivalent to
/// ```
/// #include "inip/ini.hpp"
/// #include "inip/Exception.hpp"
/// ```
#include "inip/inip.hpp"

#include <cstdlib>
#include <iostream>


int main(int argc, char **argv)
{
  /// All interface are below inip namespace
  /// `using namespace inip;`

  /// Create ErrList to collect errors
  /// Equivalent to `std::vector<inip::err::Errors>`
  inip::err::ErrList errlist;

  const std::string file_name = "inip_base.ini";

  /// Set file name
  inip::ini ini(file_name);

  /// Load file and parse, a errlist is required
  /// fail_skip could judge whether skip failure or stop at the first error
  auto result = ini.load_file(errlist, true);

  /// Judge whether result is valid
  /// Or `result.get_code_err() == inip::err::ErrCode::NO_ERRORS`
  if (!result.valid()) {
    /// Due to ErrList is vector, iterator is available
    for (const auto &i : errlist) {
      std::cout << "Error: " << "Line: " << i.get_line()
                << " Code: " << i.get_code()
                << " String: " << i.to_string()
                << std::endl;
    }

    return EXIT_FAILURE;
  }


  /// Read, may throw exceptions if not exist
  try {
    ini.get("noSection", "noKey");
  }
  /// inip::err::Errors inheritance from `std::exception`
  catch (inip::err::Errors e) {
    std::cout << e.to_string() << std::endl;
  }

  /// Read value with default, `get_` functiond ended `_default`, `_def` will not throw exceptions, but a default balue is required
  ini.get_default("noSection", "noKey", "Default Value");

  /// Predefined `get` functions
  ini.get_bool_def("types", "boolkey", false);
  ini.get_uint_def("types", "uintkey", 100);

  /// Set value
  /// If not existed, create, or overwrite
  ini.set("newSection", "newKey", "newValue");

  /// With predefined `set` functions
  ini.set_uint("types", "newUintKey", 100);

  /// Whether section existed
  ini.contains("types");

  /// Whether key existed
  ini.exist_key("types", "existedKey");

  /// Use write to write back, inip::err::Errors may be threw
  auto write_result = ini.write();
  if (!write_result.valid()) {
    std::cout << write_result.to_string() << std::endl;

    return EXIT_FAILURE;
  }

  /// Or given another file name
  write_result = ini.write("other_file.ini");
  if (!write_result.valid()) {
    std::cout << write_result.to_string() << std::endl;

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
