#pragma once

#include <memory>
#include <string>

#include "_interfaces/data_interface.h"
#include "_interfaces/module_factory_interface.h"
#include "asset_optimization_tool.h"
#include "configuration/configuration.h"

namespace asset_optimization_tool {
class AssetOptimizationToolImpl : public AssetOptimizationTool {
 public:
  ErrorCode Initialize(const std::string& data_path,
                       const std::string& config_path) override;
  ErrorCode ReloadConfiguration() override;
  ErrorCode GetAssetNames(std::set<std::string>& asset_names) const override;
  ErrorCode SelectAssets(const std::set<std::string>& asset_names) override;
  ErrorCode SetSimulationOption(const std::string& key,
                                const std::string& value) override;
  ErrorCode Simulate(const SimulationOptions& options, SimulationId) override;
  ErrorCode SetOptimizationOption(const std::string& key,
                                  const std::string& value) override;
  ErrorCode Optimize(SimulationId simulation_id) override;
  ErrorCode Optimize() override;
  ErrorCode Test(const std::vector<std::string>& asset_names,
                 const std::vector<double>& weights,
                 const OptimizationOptions& evaluation_method) override;
  ErrorCode Evaluate(const std::vector<double>& simulation_result,
                     const OptimizationOptions& evaluation_method,
                     double result) override;

  explicit AssetOptimizationToolImpl(modules::IData* data_interface,
                                     modules::Configuration* config_);

 private:  // methods
 public:   // data members
 private:  // data members
  std::unique_ptr<modules::IData> data_interface_;
  std::unique_ptr<modules::Configuration> config_;
  std::string data_path_;
  std::string config_path_;
};
}  // namespace asset_optimization_tool