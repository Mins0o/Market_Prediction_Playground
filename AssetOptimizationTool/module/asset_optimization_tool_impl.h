#pragma once

#include "asset_optimization_tool.h"

namespace asset_optimization_tool {
class AssetOptimizationToolImpl : public AssetOptimizationTool {
 public:
  ErrorCode Initialize() override;
  ErrorCode GetAssetList(std::map<AssetId, std::string>& asset_list) override;
  ErrorCode SelectAssets(const std::set<AssetId>& asset_ids) override;
  ErrorCode SetSimulationOption(const std::string& key,
                                const std::string& value) override;
  ErrorCode Simulate(size_t num_simulations, SimulationId) override;
  ErrorCode Simulate(const SimulationOptions& options, SimulationId) override;
  ErrorCode SetOptimizationOption(const std::string& key,
                                  const std::string& value) override;
  ErrorCode Optimize(SimulationId simulation_id) override;
  ErrorCode Optimize() override;
  ErrorCode Test(const std::vector<AssetId>& asset_ids,
                 const std::vector<double>& weights,
                 const OptimizationOptions& evaluation_method) override;
  ErrorCode Evaluate(const std::vector<double>& simulation_result,
                     const OptimizationOptions& evaluation_method,
                     double result) override;

  static std::unique_ptr<AssetOptimizationTool> Create();
};
}  // namespace asset_optimization_tool