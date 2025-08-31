#ifndef __INIP_TOOLS_HPP__
#define __INIP_TOOLS_HPP__

#include <string>
#include <vector>

namespace inip {
namespace Tools {

std::string get_build_info();

std::vector<std::string> split(const std::string &str, const char delim);

}
}


#endif
