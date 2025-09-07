#ifndef __INIP_EXCEPTION_HPP__
#define __INIP_EXCEPTION_HPP__

#include "config.h"

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
  
#ifdef INIP_USE_IMPORT
  IMPORT_ERROR = 400,
  IMPORT_TOO_MUCH_TOKEN = 401,
  IMPORT_INVALID = 402,
  IMPORT_INVALID_LOCATION = 403,
#endif
  
  GET_ERROR = 300,
  NO_SUCH_KEY = 301,
  INVALID_VALUE = 302,
};

class Errors : public std::exception
{
public:
  ErrCode code;
  unsigned int line;
  std::string file_name;

  Errors();
  Errors(const ErrCode code, const unsigned int line = 0, const std::string &file_name = "");
  const char* what() const throw();
  int get_code();
  void operator=(const inip::err::Errors &err);
};

}
}

#endif
