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
  if (ErrorCode err = config_->LoadConfiguration(config_path);
      err != ErrorCode::kSuccess) {
    return err;
  }
  return ErrorCode::kSuccess;
}

ErrorCode AssetOptimizationToolImpl::ReloadConfiguration() {
  // Reload the configuration
  if (ErrorCode err = config_->LoadConfiguration(config_path_);
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
  config_->SetAssetSelection(asset_names);
  return ErrorCode::kSuccess;
}

ErrorCode AssetOptimizationToolImpl::SetSimulationOption(
    const std::string &key, const std::string &value) {
  // Set the simulation option
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
  // 1. Retrieve selection stock data
  std::set<std::string> asset_selections;
  std::vector<const modules::IAsset *> selected_data;
  if (ErrorCode err = config_->GetAssetSelection(asset_selections);
      err != ErrorCode::kSuccess) {
    return err;
  }
  if (ErrorCode err =
          data_interface_->GetAssetDataByNames(asset_selections, selected_data);
      err != ErrorCode::kSuccess) {
    return err;
  }

  // 2. Align data of the stocks
  // 3. Should I give this to an optimizer or a simulator?
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
    modules::IData *data_interface, modules::Configuration *config_)
    : data_interface_(std::unique_ptr<modules::IData>(data_interface)),
      config_(std::unique_ptr<modules::Configuration>(config_)) {}

// static std::unique_ptr<AssetOptimizationTool> Create()
// is implemented in module_factory_impl.h

}  // namespace asset_optimization_tool