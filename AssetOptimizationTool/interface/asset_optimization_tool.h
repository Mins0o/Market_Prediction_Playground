#pragma once

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "types.h"

namespace asset_optimization_tool {

class AssetOptimizationTool {
 public:
  virtual ~AssetOptimizationTool() = default;

  virtual ErrorCode Initialize(const std::string& data_path) = 0;
  virtual ErrorCode GetAssetList(
      /*O*/ std::map<AssetId, std::string>& asset_name_list) = 0;
  virtual ErrorCode SelectAssets(/*I*/ const std::set<AssetId>& asset_ids) = 0;
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
      /*I*/ const std::vector<AssetId>& asset_ids,
      /*I*/ const std::vector<double>& weights,
      /*O*/ const OptimizationOptions& evaluation_method) = 0;
  virtual ErrorCode Evaluate(/*I*/ const std::vector<double>& simulation_result,
                             /*I*/ const OptimizationOptions& evaluation_method,
                             /*O*/ double result) = 0;

  static AssetOptimizationTool* Create();
};
}  // namespace asset_optimization_tool