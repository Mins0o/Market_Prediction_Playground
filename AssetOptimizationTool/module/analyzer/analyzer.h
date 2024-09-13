#pragma once

#include "analyzer_interface.h"

namespace asset_optimization_tool {
class Analyzer : public AnalyzerInterface {
 public:
  void analyze() override;
};
}  // namespace asset_optimization_tool