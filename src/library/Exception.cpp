#include "inip/Exception.hpp"

#include <string>
#include <sstream>

inip::err::Errors::Errors(){}

inip::err::Errors::Errors(const inip::err::ErrCode code, const unsigned int line, const std::string &file_name)
{
  this->code = code;
  this->line = line;
  this->file_name = file_name;
  this->msg = std::string("");
}

const char* inip::err::Errors::what() const throw()
{
  std::stringstream ss("");
  ss << "Inip Error. " << "At line: " << this->line << ", file: " << this->file_name << ", code: " << static_cast<int>(this->code) << ".";
  this->msg = ss.str();
  return this->msg.c_str();
}

const int inip::err::Errors::get_code() const
{
  return static_cast<int>(this->code);
}

const inip::err::ErrCode inip::err::Errors::get_code_err() const
{
  return this->code;
}

const std::string inip::err::Errors::get_file() const
{
  return this->file_name;
}

const inip::Types::LineNum inip::err::Errors::get_line() const
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

inip::err::Errors inip::err::Errors::set_line(const inip::Types::LineNum line)
{
  this->line = line;
  return *this;
}

inip::err::Errors inip::err::Errors::set_file(const std::string &file_name)
{
  this->file_name = file_name;
  return *this;
}
