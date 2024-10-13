#pragma once

#include <set>

#include "_interfaces/configuration_interface.h"
#include "types.h"

namespace asset_optimization_tool::modules {
class AssetSelections : public IAssetSelections {
 public:
  AssetSelections() = default;
  AssetSelections(const std::set<std::string>& asset_names)
      : asset_names_(asset_names) {};
  ErrorCode AddToSelection(const std::string& asset_name);
  ErrorCode AddBulkToSelection(const std::set<std::string>& asset_names);
  ErrorCode RemoveFromSelection(const std::string& asset_name);
  ErrorCode RemoveBulkFromSelection(const std::set<std::string>& asset_names);
  ErrorCode GetSelection(std::set<std::string>& asset_names) const;
  ErrorCode ClearSelection();

 private:
  std::set<std::string> asset_names_;
};

class SimulationConfigurations {
 public:
  enum class PortfolioMixingStrategy {
    DailyRebalancing = 0,
    IntervalRebalancing,
    MonthlyRebalancing,
    NeverRebalance
  };
  SimulationConfigurations() = default;
  ErrorCode SetRepCount(size_t rep_count);
  ErrorCode SetMixingStrategy(PortfolioMixingStrategy strategy);

 private:
  size_t rep_count_ = 10'000;
};

class EvaluationConfiguration {
 public:
  enum class RiskStrategy {
    StandardDeviation = 0,
    LossLikelihood,
  };
  enum class ExpectedReturnStrategy { SimpleMean = 0, DiscountedMean };
  EvaluationConfiguration() = default;
  ErrorCode SetRiskStrategy(RiskStrategy strategy);
  ErrorCode SetExpectedReturnStrategy(ExpectedReturnStrategy strategy);

 private:
  RiskStrategy risk_strategy_ = RiskStrategy::StandardDeviation;
  ExpectedReturnStrategy expected_return_strategy_ =
      ExpectedReturnStrategy::SimpleMean;
};

class Configuration : public IConfiguration {
 public:
  Configuration() = default;
  ErrorCode SetAssetSelections(const AssetSelections& asset_selections);
  ErrorCode SetSimulationConfigurations(
      const SimulationConfigurations& simulation_configurations);
  ErrorCode SetEvaluationConfiguration(
      const EvaluationConfiguration& evaluation_configuration);
  ErrorCode GetAssetSelections(AssetSelections& asset_selections) const;
  ErrorCode GetSimulationConfigurations(
      SimulationConfigurations& simulation_configurations) const;
  ErrorCode GetEvaluationConfiguration(
      EvaluationConfiguration& evaluation_configuration) const;

 private:
  AssetSelections asset_selections_;
  SimulationConfigurations simulation_configurations_;
  EvaluationConfiguration evaluation_configuration_;
};
}  // namespace asset_optimization_tool::modules