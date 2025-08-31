#ifndef __INIP_INIMGR_HPP__
#define __INIP_INIMGR_HPP__

#include "inip/Exception.hpp"
#include "inip/Section.hpp"

#include <string>
#include <fstream>
#include <vector>
#include <map>

namespace inip {

class IniMgr
{
private:
  std::string file_name;
  std::fstream file_obj;
  std::map<std::string, Section> data;
  std::vector<std::string> sec_list;
  friend class ini;

public:
  IniMgr(const std::string &file_name);
  ~IniMgr();
  bool is_section_exists(const std::string &secname);
  bool line_filter(const std::string &input, std::string &output);
  err::Errors parse_file();
};

}

#endif
