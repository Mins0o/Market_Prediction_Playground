#include "configuration/configuration.h"

#include <json/json.h>

#include <fstream>
#include <iostream>

namespace asset_optimization_tool::modules {
// ------------------- SimulationConfigurations -------------------
ErrorCode SimulationConfigurations::SetSimulationOption(
    const std::string& key, const std::string& value) {
  if (key == "RepetitionCount") {
    return SetRepetitionCount(value);
  } else if (key == "PortfolioMixingStrategy") {
    return SetPortfolioMixingStrategy(value);
  }
  return ErrorCode::kInvalidArgument;
}
ErrorCode SimulationConfigurations::SetRepetitionCount(std::string count) {
  try {
    rep_count = std::stoul(count);
  } catch (const std::invalid_argument& e) {
    std::cerr
        << "SimulationConfigurations::SetRepetitionCount: Invalid string - "
        << count << std::endl;
    return ErrorCode::kInvalidArgument;
  }
  return ErrorCode::kSuccess;
}
ErrorCode SimulationConfigurations::SetPortfolioMixingStrategy(
    std::string strategy) {
  if (strategy == "DailyRebalancing") {
    portfolio_mixing_strategy = PortfolioMixingStrategy::DailyRebalancing;
  } else if (strategy == "IntervalRebalancing") {
    portfolio_mixing_strategy = PortfolioMixingStrategy::IntervalRebalancing;
  } else if (strategy == "MonthlyRebalancing") {
    portfolio_mixing_strategy = PortfolioMixingStrategy::MonthlyRebalancing;
  } else if (strategy == "NeverRebalance") {
    portfolio_mixing_strategy = PortfolioMixingStrategy::NeverRebalance;
  } else {
    std::cerr
        << "SimulationConfigurations::SetPortfolioMixingStrategy: Invalid "
           "strategy - "
        << strategy << std::endl;
    return ErrorCode::kInvalidArgument;
  }
  return ErrorCode::kSuccess;
}
// ------------------- EvaluationConfigurations -------------------
ErrorCode EvaluationConfigurations::SetEvaluationOption(
    const std::string& key, const std::string& value) {
  if (key == "RiskStrategy") {
    return SetRiskStrategy(value);
  } else if (key == "ExpectedReturnStrategy") {
    return SetExpectedReturnStrategy(value);
  }
  return ErrorCode::kInvalidArgument;
}
ErrorCode EvaluationConfigurations::SetRiskStrategy(std::string strategy) {
  if (strategy == "StandardDeviation") {
    risk_strategy = RiskStrategy::StandardDeviation;
  } else if (strategy == "LossLikelihood") {
    risk_strategy = RiskStrategy::LossLikelihood;
  } else {
    std::cerr
        << "EvaluationConfigurations::SetRiskStrategy: Invalid strategy - "
        << strategy << std::endl;
    return ErrorCode::kInvalidArgument;
  }
  return ErrorCode::kSuccess;
}
ErrorCode EvaluationConfigurations::SetExpectedReturnStrategy(
    std::string strategy) {
  if (strategy == "SimpleMean") {
    expected_return_strategy = ExpectedReturnStrategy::SimpleMean;
  } else if (strategy == "DiscountedMean") {
    expected_return_strategy = ExpectedReturnStrategy::DiscountedMean;
  } else if (strategy == "Median") {
    expected_return_strategy = ExpectedReturnStrategy::Median;
  } else {
    std::cerr << "EvaluationConfigurations::SetExpectedReturnStrategy: Invalid "
                 "strategy - "
              << strategy << std::endl;
    return ErrorCode::kInvalidArgument;
  }
  return ErrorCode::kSuccess;
}
// ------------------- Configuration -------------------
ErrorCode Configuration::LoadConfiguration(const std::string& config_path) {
  std::ifstream config_file(config_path);
  if (!config_file.is_open()) {
    std::cerr << "Configuration::LoadConfiguration: File not found - "
              << config_path << std::endl;
    return ErrorCode::kFileNotFound;
  }
  Json::Value root;
  Json::CharReaderBuilder builder;
  std::string errs;
  if (!Json::parseFromStream(builder, config_file, &root, &errs)) {
    std::cerr << "Configuration::LoadConfiguration: Error parsing JSON - "
              << errs << std::endl;
    return ErrorCode::kCannotReadFile;
  }
  config_file.close();

  if (root.isMember("AssetSelection")) {
    const Json::Value& asset_selection = root["AssetSelection"];
    std::set<std::string> asset_names;
    for (const auto& asset_name : asset_selection) {
      asset_names.insert(asset_name.asString());
    }
    SetAssetSelection(asset_names);
  }

  if (root.isMember("SimulationOptions")) {
    const Json::Value& simulation_options = root["SimulationOptions"];
    for (const auto& key : simulation_options.getMemberNames()) {
      SetSimulationOption(key, simulation_options[key].asString());
    }
  }

  if (root.isMember("OptimizationOptions")) {
    const Json::Value& optimization_options = root["OptimizationOptions"];
    for (const auto& key : optimization_options.getMemberNames()) {
      SetOptimizationOption(key, optimization_options[key].asString());
    }
  }

  return ErrorCode::kSuccess;
}
ErrorCode Configuration::GetConfigurationKeys(
    std::set<std::string>& keys) const {
  return ErrorCode::kSuccess;
}
ErrorCode Configuration::GetConfigurationValue(const std::string& key,
                                               std::string& value) const {
  return ErrorCode::kSuccess;
}
ErrorCode Configuration::SetAssetSelection(
    const std::set<std::string>& asset_names) {
  asset_selection_ = asset_names;
  return ErrorCode::kSuccess;
}
ErrorCode Configuration::SetSimulationOption(const std::string& key,
                                             const std::string& value) {
  if (simulation_configurations_.SetSimulationOption(key, value) !=
      ErrorCode::kSuccess) {
    return ErrorCode::kInvalidArgument;
  }
  return ErrorCode::kSuccess;
}
ErrorCode Configuration::SetOptimizationOption(const std::string& key,
                                               const std::string& value) {
  return ErrorCode::kSuccess;
}
}  // namespace asset_optimization_tool::modules