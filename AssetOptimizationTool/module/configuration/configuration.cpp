#include "configuration/configuration.h"

#include <fstream>
#include <iostream>
#include <rapidfuzz/fuzz.hpp>
#include <string_view>

namespace {
std::string_view lower(const std::string_view& input) {
  std::string lower_case;
  lower_case.reserve(input.size());
  for (const auto& c : input) {
    lower_case.push_back(std::tolower(c));
  }
  return lower_case;
}
std::string_view fuzzy_match(const std::string_view& target,
                             std::set<std::string_view> string_set) {
  std::vector<std::pair<std::string, double>> results;
  rapidfuzz::fuzz::CachedRatio<char> scorer(lower(target));
  for (const std::string_view& choice : string_set) {
    double score = scorer.similarity(lower(choice).data());
    results.emplace_back(choice, score);
  }
  std::sort(results.begin(), results.end(),
            [](auto a, auto b) { return a.second > b.second; });
  return results[0].first;
}

}  // namespace

namespace asset_optimization_tool::modules {
// ------------------- SimulationConfigurations -------------------
ErrorCode SimulationConfigurations::SetRepetition(
    const std::string& repetition) {
  try {
    repetition_ = std::stoul(repetition);
  } catch (const std::exception& e) {
    std::cerr << "Failed to set repetition count: " << e.what() << std::endl;
    return ErrorCode::kInvalidArgument;
  }
  return ErrorCode::kSuccess;
}
ErrorCode SimulationConfigurations::SetRepetition(size_t repetition) {
  repetition_ = repetition;
  return ErrorCode::kSuccess;
}
ErrorCode SimulationConfigurations::SetPortfolioMixingStrategy(
    const std::string& strategy) {
  std::set<std::string_view> valid_strategies;
  for (const auto& [strategy_name, _] : portfolio_mixing_strategy_map_) {
    valid_strategies.insert(strategy_name);
  }
  auto match = fuzzy_match(strategy, valid_strategies);
  if (match.empty()) {
    std::cerr << "Invalid portfolio mixing strategy: " << strategy << std::endl;
    return ErrorCode::kInvalidArgument;
  }
  portfolio_mixing_strategy_ = portfolio_mixing_strategy_map_.at(match);
  return ErrorCode::kSuccess;
}
ErrorCode SimulationConfigurations::GetRepetition(size_t& repetition) const {
  repetition = repetition_;
  return ErrorCode::kSuccess;
}
ErrorCode SimulationConfigurations::GetPortfolioMixingStrategy(
    PortfolioMixingStrategy& portfolio_mixing_strategy) const {
  portfolio_mixing_strategy = portfolio_mixing_strategy_;
  return ErrorCode::kSuccess;
}
std::map<std::string_view, SimulationConfigurations::PortfolioMixingStrategy>
    SimulationConfigurations::portfolio_mixing_strategy_map_ = {
        {"DailyRebalancing", PortfolioMixingStrategy::DailyRebalancing},
        {"IntervalRebalancing", PortfolioMixingStrategy::IntervalRebalancing},
        {"MonthlyRebalancing", PortfolioMixingStrategy::MonthlyRebalancing},
        {"NeverRebalance", PortfolioMixingStrategy::NeverRebalance}};
// ------------------- EvaluationConfigurations -------------------
ErrorCode EvaluationConfigurations::SetRiskStrategy(
    const std::string& risk_strategy) {
  std::set<std::string_view> valid_strategies;
  for (const auto& [strategy_name, _] : risk_strategy_map_) {
    valid_strategies.insert(strategy_name);
  }
  auto match = fuzzy_match(risk_strategy, valid_strategies);
  if (match.empty()) {
    std::cerr << "Invalid risk strategy: " << risk_strategy << std::endl;
    return ErrorCode::kInvalidArgument;
  }
  risk_strategy_ = risk_strategy_map_.at(match);
  return ErrorCode::kSuccess;
}
ErrorCode EvaluationConfigurations::SetExpectedReturnStrategy(
    const std::string& expected_return_strategy) {
  std::set<std::string_view> valid_strategies;
  for (const auto& [strategy_name, _] : expected_return_strategy_map_) {
    valid_strategies.insert(strategy_name);
  }
  auto match = fuzzy_match(expected_return_strategy, valid_strategies);
  if (match.empty()) {
    std::cerr << "Invalid expected return strategy: "
              << expected_return_strategy << std::endl;
    return ErrorCode::kInvalidArgument;
  }
  expected_return_strategy_ = expected_return_strategy_map_.at(match);
  return ErrorCode::kSuccess;
}
ErrorCode EvaluationConfigurations::GetRiskStrategy(
    RiskStrategy& risk_strategy) const {
  risk_strategy = risk_strategy_;
  return ErrorCode::kSuccess;
}
ErrorCode EvaluationConfigurations::GetExpectedReturnStrategy(
    ExpectedReturnStrategy& expected_return_strategy) const {
  expected_return_strategy = expected_return_strategy_;
  return ErrorCode::kSuccess;
}
std::map<std::string_view, EvaluationConfigurations::RiskStrategy>
    EvaluationConfigurations::risk_strategy_map_ = {
        {"StandardDeviation", RiskStrategy::StandardDeviation},
        {"LossLikelihood", RiskStrategy::LossLikelihood}};
std::map<std::string_view, EvaluationConfigurations::ExpectedReturnStrategy>
    EvaluationConfigurations::expected_return_strategy_map_ = {
        {"SimpleMean", ExpectedReturnStrategy::SimpleMean},
        {"DiscountedMean", ExpectedReturnStrategy::DiscountedMean},
        {"Median", ExpectedReturnStrategy::Median}};
// ------------------- Configuration -------------------
ErrorCode Configuration::LoadConfiguration(const std::string& config_path) {
  std::ifstream config_file(config_path);
  if (!config_file.is_open()) {
    std::cerr << "Failed to open configuration file: " << config_path
              << std::endl;
    return ErrorCode::kCannotReadFile;
  }

  Json::Value root;
  Json::CharReaderBuilder builder;
  std::string errs;
  if (!Json::parseFromStream(builder, config_file, &root, &errs)) {
    std::cerr << "Failed to parse configuration file: " << errs << std::endl;
    return ErrorCode::kInvalidData;
  }
  return LoadConfigurationJson(root);
}
ErrorCode Configuration::LoadConfigurationString(
    const std::string& config_string) {
  Json::Value root;
  Json::CharReaderBuilder builder;
  std::string errs;
  std::istringstream config_stream(config_string);
  if (!Json::parseFromStream(builder, config_stream, &root, &errs)) {
    std::cerr << "Failed to parse configuration file: " << errs << std::endl;
    return ErrorCode::kInvalidData;
  }
  return LoadConfigurationJson(root);
}
ErrorCode Configuration::LoadConfigurationJson(const Json::Value& config_json) {
  if (!config_json.isObject()) {
    std::cerr << "Invalid configuration file format: root is not an object"
              << std::endl;
    return ErrorCode::kInvalidData;
  }

  if (config_json.isMember("AssetSelection")) {
    const Json::Value& asset_selection = config_json["AssetSelection"];
    if (!asset_selection.isArray()) {
      std::cerr
          << "Invalid configuration file format: AssetSelection is missing"
          << std::endl;
      return ErrorCode::kInvalidData;
    }
    for (const auto& asset : asset_selection) {
      asset_selection_.insert(asset.asString());
    }
  }

  if (config_json.isMember("SimulationOptions")) {
    const Json::Value& simulation_options = config_json["SimulationOptions"];
    if (!simulation_options.isObject()) {
      std::cerr << "Invalid configuration file format" << std::endl;
      return ErrorCode::kInvalidData;
    }
    if (simulation_options.isMember("repetitions")) {
      const Json::Value& repetition = simulation_options["repetitions"];
      try {
        simulation_configurations_.SetRepetition(repetition.asUInt64());
      } catch (const std::exception& e) {
        std::cerr << "Failed to set repetition count: " << e.what()
                  << std::endl;
        return ErrorCode::kInvalidArgument;
      }
    }
    if (simulation_options.isMember("portfolio_mix_strategy")) {
      const Json::Value& strategy =
          simulation_options["portfolio_mix_strategy"];
      const std::string& strategy_string = strategy.asString();
      if (ErrorCode err = simulation_configurations_.SetPortfolioMixingStrategy(
              strategy_string);
          err != ErrorCode::kSuccess) {
        return err;
      }
    }
  }
  return ErrorCode::kSuccess;
}
ErrorCode Configuration::GetAssetSelection(
    std::set<std::string>& asset_names) const {
  asset_names = asset_selection_;
  return ErrorCode::kSuccess;
}
ErrorCode Configuration::GetSimulationOption(
    SimulationConfigurations& simulation_configurations) const {
  simulation_configurations = simulation_configurations_;
  return ErrorCode::kSuccess;
}
ErrorCode Configuration::GetEvaluationOption(
    EvaluationConfigurations& evaluation_configurations) const {
  evaluation_configurations = evaluation_configurations_;
  return ErrorCode::kSuccess;
}

ErrorCode Configuration::SetAssetSelection(
    const std::set<std::string>& asset_names) {
  asset_selection_ = asset_names;
  return ErrorCode::kSuccess;
}
}  // namespace asset_optimization_tool::modules