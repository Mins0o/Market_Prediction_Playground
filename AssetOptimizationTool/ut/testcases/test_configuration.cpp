#include <json/json.h>

#include "./configurations.h"
#include "configuration/configuration.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "types.h"

using ::asset_optimization_tool::ErrorCode;
using namespace ::asset_optimization_tool::modules;

namespace {
std::set<std::string> kAssetNames = {"asset1", "asset2", "3asset"};
}

TEST(ConfigurationTest, LoadConfiguration) {
  Configuration config;
  SimulationConfigurations simulation_options;
  SimulationConfigurations::PortfolioMixingStrategy mixing_strategy;
  EvaluationConfigurations evaluation_options;
  EvaluationConfigurations::RiskStrategy risk_strategy;
  EvaluationConfigurations::ExpectedReturnStrategy expected_return_strategy;
  std::string config_path = "./build/ut/config_example.json";
  std::set<std::string> asset_names;

  ASSERT_EQ(config.LoadConfiguration(config_path), ErrorCode::kSuccess);
  ASSERT_EQ(config.GetAssetSelection(asset_names), ErrorCode::kSuccess);
  ASSERT_THAT(asset_names, ::testing::UnorderedElementsAreArray(kAssetNames));
}

TEST(ConfigurationTest, LoadNonExistentFile) {
  asset_optimization_tool::modules::Configuration config;
  std::string config_path = "wrong_path.json";

  ASSERT_EQ(config.LoadConfiguration(config_path), ErrorCode::kCannotReadFile);
}

TEST(ConfigurationTest, LoadInvalidRepetition) {
  asset_optimization_tool::modules::Configuration config;

  ASSERT_EQ(
      config.LoadConfigurationString(config_test::kInvalidRepetitionConfig),
      ErrorCode::kInvalidArgument);
}

TEST(ConfigurationTest, LoadFuzzySimulationStrategy) {
  Configuration config;
  SimulationConfigurations simulation_options;
  SimulationConfigurations::PortfolioMixingStrategy strategy;

  ASSERT_EQ(config.LoadConfigurationString(
                config_test::kFuzzySimulationStrategyConfig),
            ErrorCode::kSuccess);
  ASSERT_EQ(config.GetSimulationOption(simulation_options),
            ErrorCode::kSuccess);
  ASSERT_EQ(simulation_options.GetPortfolioMixingStrategy(strategy),
            ErrorCode::kSuccess);
  ASSERT_EQ(strategy,
            asset_optimization_tool::modules::SimulationConfigurations::
                PortfolioMixingStrategy::NeverRebalance);
}
