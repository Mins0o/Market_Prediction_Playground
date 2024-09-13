#pragma once

namespace asset_optimization_tool::modules {
class AnalyzerInterface {
 public:
  virtual void analyze() = 0;
};
}  // namespace asset_optimization_tool::modules