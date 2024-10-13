#pragma once

#include <set>

#include "_interfaces/configuration_interface.h"
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
 private:
  size_t rep_count = 10'000;
  PortfolioMixingStrategy portfolio_mixing_strategy =
      PortfolioMixingStrategy::DailyRebalancing;
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

class Configuration : public IConfiguration {
 public:
  ErrorCode LoadConfiguration(const std::string& config_path) override;
  ErrorCode GetConfigurationKeys(std::set<std::string>& keys) const override;
  ErrorCode GetConfigurationValue(const std::string& key,
                                  std::string& value) const override;

  ErrorCode SetAssetSelection(
      const std::set<std::string>& asset_names) override;
  ErrorCode SetSimulationOption(const std::string& key,
                                const std::string& value) override;
  ErrorCode SetOptimizationOption(const std::string& key,
                                  const std::string& value) override;

 private:
 private:
  std::set<std::string> asset_selection_;
  SimulationConfigurations simulation_configurations_;
  EvaluationConfigurations evaluation_configurations_;
};
}  // namespace asset_optimization_tool::modules