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

inip::err::ErrCode inip::err::Errors::get_code_err()
{
  return this->code;
}

std::string inip::err::Errors::get_file()
{
  return this->file_name;
}

unsigned int inip::err::Errors::get_line()
{
  return this->line;
}

void inip::err::Errors::operator=(const inip::err::Errors &err)
{
  this->code = err.code;
  this->line = err.line;
  this->file_name = err.file_name;
}

inip::err::Errors inip::err::Errors::set_code(const inip::err::ErrCode err)
{
  this->code = err;
  return *this;
}

inip::err::Errors inip::err::Errors::set_line(const unsigned int line)
{
  this->line = line;
  return *this;
}

inip::err::Errors inip::err::Errors::set_file(const std::string &file_name)
{
  this->file_name = file_name;
  return *this;
}
