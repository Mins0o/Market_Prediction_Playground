#include "configuration.h"

namespace asset_optimization_tool::modules {
// ------------------- AssetSelections -------------------
ErrorCode AssetSelections::AddToSelection(const std::string& asset_name) {
  asset_names_.insert(asset_name);
  return ErrorCode::kSuccess;
}
ErrorCode AssetSelections::AddBulkToSelection(
    const std::set<std::string>& asset_names) {
  asset_names_.insert(asset_names.begin(), asset_names.end());
  return ErrorCode::kSuccess;
}
ErrorCode AssetSelections::RemoveFromSelection(const std::string& asset_name) {
  asset_names_.erase(asset_name);
  return ErrorCode::kSuccess;
}
ErrorCode AssetSelections::RemoveBulkFromSelection(
    const std::set<std::string>& asset_names) {
  for (const auto& id : asset_names) {
    asset_names_.erase(id);
  }
  return ErrorCode::kSuccess;
}
ErrorCode AssetSelections::GetSelection(
    std::set<std::string>& asset_names) const {
  asset_names = asset_names_;
  return ErrorCode::kSuccess;
}
ErrorCode AssetSelections::ClearSelection() {
  asset_names_.clear();
  return ErrorCode::kSuccess;
}
// ------------------- SimulationConfigurations -------------------
ErrorCode SimulationConfigurations::SetRepCount(size_t rep_count) {
  rep_count_ = rep_count;
  return ErrorCode::kSuccess;
}
ErrorCode SimulationConfigurations::SetMixingStrategy(
    PortfolioMixingStrategy strategy) {
  return ErrorCode::kSuccess;
}
// ------------------- EvaluationConfiguration -------------------
ErrorCode EvaluationConfiguration::SetRiskStrategy(RiskStrategy strategy) {
  risk_strategy_ = strategy;
  return ErrorCode::kSuccess;
}
ErrorCode EvaluationConfiguration::SetExpectedReturnStrategy(
    ExpectedReturnStrategy strategy) {
  expected_return_strategy_ = strategy;
  return ErrorCode::kSuccess;
}
}  // namespace asset_optimization_tool::modules