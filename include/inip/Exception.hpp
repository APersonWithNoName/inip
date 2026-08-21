/**
 * @file Exception.hpp
 * @brief Class of exceptions and errors
 * @author APersonWithNoName
 * @version 0.1.0
 * @date 2026-08-20
 */
#ifndef __INIP_EXCEPTION_HPP__
#define __INIP_EXCEPTION_HPP__

#include "inip/Types.hpp"

#include <exception>
#include <string>
#include <vector>

namespace inip
{

namespace err
{

/**
 * @brief inip error codes enum
 */
enum class ErrCode {
  /// Normal, no errors
  NO_ERRORS = 0,

  /// File

  /// All file error
  FILE_ERROR = 100,
  /// Cannot open the file
  CANNOT_OPEN_FILE = 101,
  /// Cannot write stream to file
  CANNOT_WRITE_FILE = 102,
  /// Cannot flush to disk
  DISK_WRITE_FAILED = 103,

  /// INI hrammar

  /// All grammars
  GRAMMAR_ERROR = 200,
  /// key already existed
  KEY_EXISTS = 201,
  /// key is empty
  KEY_EMPTY = 202,
  /// miss left [
  MISS_LEFT_BRACKETS = 203,
  /// miss right ]
  MISS_RIGHT_BRACKETS = 207,
  /// miss =
  MISS_EQUAL = 204,
  /// section already existed
  SEC_EXISTS = 205,
  /// section name is empty
  SEC_EMPTY = 206,

  /// Import

  /// All import errors
  IMPORT_ERROR = 400,

  /// Get value

  /// All get errors
  GET_ERROR = 300,
  /// No such key
  NO_SUCH_KEY = 301,
  /// No such section
  NO_SUCH_SECTION = 304,
  /// invalid input value
  INVALID_VALUE = 302,
};

/**
 * @brief Main class of errors and exceptions, can be threw
 */
class Errors : public std::exception
{
public:
  ErrCode code;
  inip::Types::LineNum line;
  std::string file_name;
  mutable std::string msg;

  Errors();
  Errors(const ErrCode code, const unsigned int line = 0, const std::string &file_name = "");

  /**
   * @brief what(): the necessary override of thriwing exception
   */
  const char *what() const throw() override;

  /**
   * @brief to_string translate code to humanized string
   */
  const char *to_string(void) const;

  /**
   * @brief get_code_err Return the code with @ref inip::err::ErrCode format
   */
  const ErrCode get_code_err(void) const;

  /**
   * @brief get_code Return the code with int format
   */
  const int get_code(void) const;

  /**
   * @brief get_file Get the file where the errors occurred
   */
  const std::string get_file(void) const;

  /**
   * @brief get_line Get the line number
   */
  const inip::Types::LineNum get_line(void) const;

  /**
   * @brief valid Return whether the error status is normal,
   *
   * @return If no errors in this class, return `true`
   *
   * @details Equals to `get_code_err() == NO_ERRORS`
   */
  const bool valid(void) const;

  inip::err::Errors set_code(const inip::err::ErrCode err);
  inip::err::Errors set_line(const inip::Types::LineNum line);
  inip::err::Errors set_file(const std::string &file_name);

  Errors &operator=(const inip::err::Errors &err);
};

/**
 * @brief An alias to error list which collect errors
 */
typedef std::vector<Errors> ErrList;

} // namespace err
} // namespace inip

#endif
