#pragma once

namespace asset_optimization_tool::modules {
class IAnalyzer {
 public:
  virtual void analyze() = 0;
};
}  // namespace asset_optimization_tool::modules