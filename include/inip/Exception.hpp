#ifndef __INIP_EXCEPTION_HPP__
#define __INIP_EXCEPTION_HPP__

#include "inip/Types.hpp"

#include <exception>
#include <string>

namespace inip {

namespace err {

enum class ErrCode
{
  NO_ERRORS = 0,

  FILE_ERROR = 100,
  CANNOT_OPEN_FILE = 101,

  GRAMMAR_ERROR = 200,
  KEY_EXISTS = 201,
  KEY_EMPTY = 202,
  MISS_BRACKETS = 203,
  MISS_EQUAL = 204,
  SEC_EXISTS = 205,
  SEC_EMPTY = 206,
  
  IMPORT_ERROR = 400,
  IMPORT_TOO_MUCH_TOKEN = 401,
  IMPORT_INVALID = 402,
  IMPORT_INVALID_LOCATION = 403,
  
  GET_ERROR = 300,
  NO_SUCH_KEY = 301,
  NO_SUCH_SECTION = 304,
  INVALID_VALUE = 302,
};

class Errors : public std::exception
{
public:
  ErrCode code;
  inip::Types::LineNum line;
  std::string file_name;
  mutable std::string msg;

  Errors();
  Errors(const ErrCode code, const unsigned int line = 0, const std::string &file_name = "");

  const char* what() const throw();

  const ErrCode get_code_err() const;
  const int get_code() const;
  const std::string get_file() const;
  const inip::Types::LineNum get_line() const;

  inip::err::Errors set_code(const inip::err::ErrCode err);
  inip::err::Errors set_line(const inip::Types::LineNum line);
  inip::err::Errors set_file(const std::string &file_name);

  void operator=(const inip::err::Errors &err);
};

}
}

#endif
