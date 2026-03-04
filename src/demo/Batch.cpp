#include "Batch.hpp"
#include <fstream>
#include <iostream>

namespace inip {
namespace demo {
namespace ui {

void Batch::init()
{
  this->cmdexec.env.type = inip::demo::core::EnvironmentType::COMMAND;
}

int Batch::run()
{
  if (this->bc.file != "") {
    std::ifstream fin;
    fin.open(this->bc.file, std::ios::in);

    if (!fin.is_open()) {
      std::cerr << "Cannot open file: " << this->bc.file << std::endl;
      return -1;
    }

    std::string line;
    while (std::getline(fin, line)) {
      this->cmdexec.exec(line);
    }
  }
 
  return this->cmdexec.exec(this->bc.command).second;
}

void Batch::configure(const BatchConfigure &bc)
{
  this->bc.command = bc.command;
  this->bc.file = bc.file;
}

} // namespace ui
} // namespace demo
} // namespace inip
