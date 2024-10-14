#pragma once

#include <rapidfuzz/fuzz.hpp>
#include <set>
#include <string>

#include "types.h"

namespace asset_optimization_tool::modules {
class SimulationConfigurations {
  enum class PortfolioMixingStrategy {
    DailyRebalancing = 0,
    IntervalRebalancing,
    MonthlyRebalancing,
    NeverRebalance
  };

 public:
  ErrorCode SetRepetition(std::string repetition);
  ErrorCode SetRepetition(size_t repetition);
  ErrorCode GetRepetition(size_t& repetition) const {
    repetition = repetition_;
  }
  ErrorCode SetPortfolioMixingStrategy(std::string strategy);
  ErrorCode GetPortfolioMixingStrategy(
      PortfolioMixingStrategy& portfolio_mixing_strategy) const {
    portfolio_mixing_strategy = portfolio_mixing_strategy_;
  }

 private:
  size_t repetition_ = 10'000;
  PortfolioMixingStrategy portfolio_mixing_strategy_ =
      PortfolioMixingStrategy::DailyRebalancing;
  std::map<const char*, PortfolioMixingStrategy> portfolio_mixing_strategy_map_{
      {"DailyRebalancing", PortfolioMixingStrategy::DailyRebalancing},
      {"IntervalRebalancing", PortfolioMixingStrategy::IntervalRebalancing},
      {"MonthlyRebalancing", PortfolioMixingStrategy::MonthlyRebalancing},
      {"NeverRebalance", PortfolioMixingStrategy::NeverRebalance}};
};

class EvaluationConfigurations {
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

 public:
 private:
  RiskStrategy risk_strategy = RiskStrategy::StandardDeviation;
  ExpectedReturnStrategy expected_return_strategy =
      ExpectedReturnStrategy::SimpleMean;
};

class Configuration {
 public:
  ErrorCode LoadConfiguration(const std::string& config_path);
  ErrorCode GetAssetSelection(std::set<std::string>& asset_names) const;
  ErrorCode GetSimulationOption(
      SimulationConfigurations& simulation_configurations) const;
  ErrorCode GetOptimizationOption(
      EvaluationConfigurations& evaluation_configurations) const;

  ErrorCode SetAssetSelection(const std::set<std::string>& asset_names);
  ErrorCode SetSimulationOption(const std::string& key,
                                const std::string& value);
  ErrorCode SetOptimizationOption(const std::string& key,
                                  const std::string& value);

 private:
 private:
  std::set<std::string> asset_selection_;
  SimulationConfigurations simulation_configurations_;
  EvaluationConfigurations evaluation_configurations_;
};
}  // namespace asset_optimization_tool::modules