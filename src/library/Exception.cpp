#include "inip/Exception.hpp"

#include <string>

inip::err::Errors::Errors(){}

inip::err::Errors::Errors(const inip::err::ErrCode code, const unsigned int line, const std::string &file_name)
{
  this->code = code;
  this->line = line;
  this->file_name = file_name;
}

const char* inip::err::Errors::what() const throw()
{
  return "Inip Error.";
}

int inip::err::Errors::get_code()
{
  return static_cast<int>(this->code);
}

void inip::err::Errors::operator=(const inip::err::Errors &err)
{
  this->code = err.code;
  this->line = err.line;
  this->file_name = err.file_name;
}
