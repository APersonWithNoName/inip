#include "inip/Exception.hpp"
#include "inip/Types.hpp"

#include <sstream>
#include <string>

inip::err::Errors::Errors() = default;

inip::err::Errors::Errors(const inip::err::ErrCode code, const unsigned int line, const std::string &file_name)
{
  this->code = code;
  this->line = line;
  this->file_name = file_name;
  this->msg = std::string("");
}

const char *inip::err::Errors::what() const throw()
{
  std::stringstream ss("");
  ss << "Inip Error. " << "At line: " << this->line << ", file: " << this->file_name << ", code: " << static_cast<int>(this->code) << ".";
  this->msg = ss.str();
  return this->msg.c_str();
}

const int inip::err::Errors::get_code(void) const
{
  return static_cast<int>(this->code);
}

const inip::err::ErrCode inip::err::Errors::get_code_err(void) const
{
  return this->code;
}

const std::string inip::err::Errors::get_file(void) const
{
  return this->file_name;
}

const inip::Types::LineNum inip::err::Errors::get_line(void) const
{
  return this->line;
}

inip::err::Errors &inip::err::Errors::operator=(const inip::err::Errors &err)
{
  if (this != &err) {
    this->code = err.code;
    this->line = err.line;
    this->file_name = err.file_name;
  }
  return *this;
}

const bool inip::err::Errors::valid(void) const
{
  return this->code == inip::err::ErrCode::NO_ERRORS;
}

const char *inip::err::Errors::to_string(void) const
{
#define _GEN_CASE(c, s)       \
  case inip::err::ErrCode::c: \
    return s;

  switch (this->code) {
    _GEN_CASE(NO_ERRORS, "No errors")

    _GEN_CASE(FILE_ERROR, "File operations error")
    _GEN_CASE(CANNOT_OPEN_FILE, "Cannot open file")
    _GEN_CASE(CANNOT_WRITE_FILE, "Cannot write file")
    _GEN_CASE(DISK_WRITE_FAILED, "Cannot write file to disk")

    _GEN_CASE(GRAMMAR_ERROR, "INI grammar error")
    _GEN_CASE(KEY_EXISTS, "Key has already existed")
    _GEN_CASE(KEY_EMPTY, "Key cannot be empty")
    _GEN_CASE(MISS_LEFT_BRACKETS, "Miss left bracket '['")
    _GEN_CASE(MISS_RIGHT_BRACKETS, "Miss right brackets '['")
    _GEN_CASE(MISS_EQUAL, "Miss equal '='")
    _GEN_CASE(SEC_EXISTS, "Section has already existed")
    _GEN_CASE(SEC_EMPTY, "Section name cannot be empty")

    _GEN_CASE(IMPORT_ERROR, "Import operations error")

    _GEN_CASE(GET_ERROR, "Get operations error")
    _GEN_CASE(NO_SUCH_KEY, "No such key")
    _GEN_CASE(NO_SUCH_SECTION, "No such section")
    _GEN_CASE(INVALID_VALUE, "Invalud value")
  default:
    return "Unknown error code";
  }

#undef _GEN_CASE
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
