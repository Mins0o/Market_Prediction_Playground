#include "configuration/configuration.h"

#include <json/json.h>

#include <fstream>
#include <iostream>
#include <string_view>

namespace {
std::vector<std::pair<std::string, double>> extract(
    const std::string& query, const std::set<std::string_view>& choices,
    const double score_cutoff = 0.0) {
  std::vector<std::pair<std::string, double>> results;
  rapidfuzz::fuzz::CachedRatio<char> scorer(query);

  for (const auto& choice : choices) {
    double score = scorer.similarity(choice, score_cutoff);
    if (score >= score_cutoff) {
      results.emplace_back(choice, score);
    }
  }
  return results;
}

std::string_view fuzzy_match(std::string target,
                             std::set<std::string_view> string_set) {
  std::vector<std::pair<std::string, double>> results;
  rapidfuzz::fuzz::CachedRatio<char> scorer(target);
  for (const auto& choice : string_set) {
    double score = scorer.similarity(choice);
    results.emplace_back(choice, score);
  }
  std::sort(results.begin(), results.end(),
            [](auto a, auto b) { return a.second > b.second; });
  return results[0].first;
}

}  // namespace

namespace asset_optimization_tool::modules {
// ------------------- SimulationConfigurations -------------------
ErrorCode SimulationConfigurations::SetRepetition(std::string repetition) {
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
    std::string strategy) {
  std::set<std::string_view> valid_strategies;
  for (const auto& [strategy_name, _] : portfolio_mixing_strategy_map_) {
    valid_strategies.insert(strategy_name);
  }
  auto match = fuzzy_match(strategy, valid_strategies);
  if (match.empty()) {
    std::cerr << "Invalid portfolio mixing strategy: " << strategy << std::endl;
    return ErrorCode::kInvalidArgument;
  }
  portfolio_mixing_strategy_ = portfolio_mixing_strategy_map_[match.data()];
  return ErrorCode::kSuccess;
}
// ------------------- EvaluationConfigurations -------------------

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

  if (!root.isObject()) {
    std::cerr << "Invalid configuration file format: root is not an object"
              << std::endl;
    return ErrorCode::kInvalidData;
  }

  if (root.isMember("AssetSelection")) {
    const Json::Value& asset_selection = root["AssetSelection"];
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

  if (root.isMember("SimulationOptions")) {
    const Json::Value& simulation_options = root["SimulationOptions"];
    if (!simulation_options.isObject()) {
      std::cerr << "Invalid configuration file format" << std::endl;
      return ErrorCode::kInvalidData;
    }
    if (simulation_options.isMember("repetitions")) {
      const Json::Value& repetition = simulation_options["RepCount"];
      simulation_configurations_.SetRepetition(repetition.asUInt64());
    }
  }
  return ErrorCode::kSuccess;
}
}  // namespace asset_optimization_tool::modules