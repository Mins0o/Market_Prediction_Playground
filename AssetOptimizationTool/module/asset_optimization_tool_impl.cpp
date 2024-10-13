#include "asset_optimization_tool_impl.h"

#include "_interfaces/module_factory_interface.h"
#include "module_factory_impl.h"

namespace asset_optimization_tool {

ErrorCode AssetOptimizationToolImpl::Initialize(
    const std::string &data_path, const std::string &config_path) {
  data_path_ = data_path;
  config_path_ = config_path;
  // Initialize the asset optimization tool
  if (ErrorCode err = data_interface_->LoadData(data_path);
      err != ErrorCode::kSuccess) {
    return err;
  }
  if (ErrorCode err = config_interface_->LoadConfiguration(config_path);
      err != ErrorCode::kSuccess) {
    return err;
  }
  return ErrorCode::kSuccess;
}

ErrorCode AssetOptimizationToolImpl::ReloadConfiguration() {
  // Reload the configuration
  if (ErrorCode err = config_interface_->LoadConfiguration(config_path_);
      err != ErrorCode::kSuccess) {
    return err;
  }
  return ErrorCode::kSuccess;
}

ErrorCode AssetOptimizationToolImpl::GetAssetNames(
    std::set<std::string> &asset_names) const {
  // Get the list of assets
  if (ErrorCode err = data_interface_->GetAssetNames(asset_names);
      err != ErrorCode::kSuccess) {
    return err;
  }
  return ErrorCode::kSuccess;
}

ErrorCode AssetOptimizationToolImpl::SelectAssets(
    const std::set<std::string> &asset_names) {
    return ErrorCode::kSuccess;
}

ErrorCode AssetOptimizationToolImpl::SetSimulationOption(
    const std::string &key, const std::string &value) {
  // Set the simulation option
  return ErrorCode::kSuccess;
}

ErrorCode AssetOptimizationToolImpl::Simulate(size_t num_simulations,
                                              SimulationId) {
  // Simulate the asset
  return ErrorCode::kSuccess;
}

ErrorCode AssetOptimizationToolImpl::Simulate(const SimulationOptions &options,
                                              SimulationId) {
  // Simulate the asset
  return ErrorCode::kSuccess;
}

ErrorCode AssetOptimizationToolImpl::SetOptimizationOption(
    const std::string &key, const std::string &value) {
  // Set the optimization option
  return ErrorCode::kSuccess;
}

ErrorCode AssetOptimizationToolImpl::Optimize(SimulationId simulation_id) {
  // Optimize the asset
  return ErrorCode::kSuccess;
}

ErrorCode AssetOptimizationToolImpl::Optimize() {
  // Optimize the asset
  return ErrorCode::kSuccess;
}

ErrorCode AssetOptimizationToolImpl::Test(
    const std::vector<std::string> &asset_names,
    const std::vector<double> &weights,
    const OptimizationOptions &evaluation_method) {
  // Test the asset
  return ErrorCode::kSuccess;
}

ErrorCode AssetOptimizationToolImpl::Evaluate(
    const std::vector<double> &simulation_result,
    const OptimizationOptions &evaluation_method, double result) {
  // Evaluate the asset
  return ErrorCode::kSuccess;
}

AssetOptimizationToolImpl::AssetOptimizationToolImpl(
    modules::IData *data_interface, modules::IConfiguration *config_interface)
    : data_interface_(std::unique_ptr<modules::IData>(data_interface)),
      config_interface_(
          std::unique_ptr<modules::IConfiguration>(config_interface)) {}

// static std::unique_ptr<AssetOptimizationTool> Create()
// is implemented in module_factory_impl.h

}  // namespace asset_optimization_tool