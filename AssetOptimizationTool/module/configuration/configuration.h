#pragma once

#include <json/json.h>

#include <set>
#include <string>

#include "types.h"

namespace asset_optimization_tool::modules {

class SimulationConfigurations {
 public:  // type definition
  enum class PortfolioMixingStrategy {
    DailyRebalancing = 0,
    IntervalRebalancing,
    MonthlyRebalancing,
    NeverRebalance
  };

 public:  // methods
  ErrorCode SetRepetition(const std::string& repetition);
  ErrorCode SetRepetition(size_t repetition);
  ErrorCode GetRepetition(size_t& repetition) const;
  ErrorCode SetPortfolioMixingStrategy(const std::string& strategy);
  ErrorCode GetPortfolioMixingStrategy(
      PortfolioMixingStrategy& portfolio_mixing_strategy) const;

 private:  // data members
  size_t repetition_ = 10'000;
  PortfolioMixingStrategy portfolio_mixing_strategy_ =
      PortfolioMixingStrategy::DailyRebalancing;
  static std::map<std::string_view, PortfolioMixingStrategy>
      portfolio_mixing_strategy_map_;
};

class EvaluationConfigurations {
 public:  // type definition
  enum class RiskStrategy {
    StandardDeviation = 0,
    LossLikelihood,
  };
  enum class ExpectedReturnStrategy {
    SimpleMean = 0,
    DiscountedMean,
    Median,
  };
  enum class EvaluationMethod {
    SharpeRatio = 0,
    SortinoRatio,
    MaximumDrawdown,
    Volatility,
    ReturnRate
  };

 public:  // methods
  ErrorCode SetRiskStrategy(const std::string& risk_strategy);
  ErrorCode SetExpectedReturnStrategy(
      const std::string& expected_return_strategy);
  ErrorCode GetRiskStrategy(RiskStrategy& risk_strategy) const;
  ErrorCode GetExpectedReturnStrategy(
      ExpectedReturnStrategy& expected_return_strategy) const;

 private:  // data members
  RiskStrategy risk_strategy_ = RiskStrategy::StandardDeviation;
  ExpectedReturnStrategy expected_return_strategy_ =
      ExpectedReturnStrategy::SimpleMean;
  static std::map<std::string_view, RiskStrategy> risk_strategy_map_;
  static std::map<std::string_view, ExpectedReturnStrategy>
      expected_return_strategy_map_;
};

class Configuration {
 public:
  ErrorCode LoadConfiguration(const std::string& config_path);
  ErrorCode LoadConfigurationString(const std::string& config_string);
  ErrorCode GetAssetSelection(std::set<std::string>& asset_names) const;
  ErrorCode GetSimulationOption(
      SimulationConfigurations& simulation_configurations) const;
  ErrorCode GetEvaluationOption(
      EvaluationConfigurations& evaluation_configurations) const;

  ErrorCode SetAssetSelection(const std::set<std::string>& asset_names);
  ErrorCode SetSimulationOption(const std::string& key,
                                const std::string& value);
  ErrorCode SetOptimizationOption(const std::string& key,
                                  const std::string& value);

 private:
  ErrorCode LoadConfigurationJson(const Json::Value& config_json);

 private:
  std::set<std::string> asset_selection_;
  SimulationConfigurations simulation_configurations_;
  EvaluationConfigurations evaluation_configurations_;
};
}  // namespace asset_optimization_tool::modules