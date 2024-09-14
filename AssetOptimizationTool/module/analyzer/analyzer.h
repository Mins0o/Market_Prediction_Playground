#pragma once

#include "_interfaces/analyzer_interface.h"

namespace asset_optimization_tool::modules {
class Analyzer : public IAnalyzer {
 public:
  void analyze() override;
};
}  // namespace asset_optimization_tool