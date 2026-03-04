// src/demo/cli/Batch.hpp
#ifndef __CLI_BATCH_HPP__
#define __CLI_BATCH_HPP__

#include <string>
#include "InipCore.hpp"
#include "Configure.hpp"

namespace inip {
namespace demo {
namespace ui {

class Batch {
public:
  inip::demo::core::CommandExec cmdexec;
  BatchConfigure bc;

  void configure(const BatchConfigure &bc);
  void init();
  int run();
};

} // namespace ui
} // namespace demo
} // namespace inip

#endif
