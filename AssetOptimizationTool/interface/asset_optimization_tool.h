#pragma once

#include <memory>
#include <set>
#include <vector>

#include "types.h"

namespace asset_optimization_tool {

class AssetOptimizationTool {
 public:
  virtual ~AssetOptimizationTool() = default;

  virtual ErrorCode Initialize(const std::string& data_path,
                               const std::string& config_path) = 0;
  virtual ErrorCode ReloadConfiguration() = 0;
  virtual ErrorCode GetAssetNames(
      /*O*/ std::set<std::string>& asset_names) const = 0;
  virtual ErrorCode SelectAssets(
      /*I*/ const std::set<std::string>& asset_names) = 0;
  virtual ErrorCode SetSimulationOption(/*I*/ const std::string& key,
                                        /*I*/ const std::string& value) = 0;
  virtual ErrorCode Simulate(/*I*/ size_t num_simulations,
                             /*O*/ SimulationId) = 0;
  virtual ErrorCode Simulate(/*I*/ const SimulationOptions& options,
                             /*O*/ SimulationId) = 0;
  virtual ErrorCode SetOptimizationOption(/*I*/ const std::string& key,
                                          /*I*/ const std::string& value) = 0;
  virtual ErrorCode Optimize(/*I*/ SimulationId simulation_id) = 0;
  virtual ErrorCode Optimize() = 0;
  virtual ErrorCode Test(
      /*I*/ const std::vector<std::string>& asset_names,
      /*I*/ const std::vector<double>& weights,
      /*O*/ const OptimizationOptions& evaluation_method) = 0;
  virtual ErrorCode Evaluate(/*I*/ const std::vector<double>& simulation_result,
                             /*I*/ const OptimizationOptions& evaluation_method,
                             /*O*/ double result) = 0;

  static AssetOptimizationTool* Create();
};
}  // namespace asset_optimization_tool