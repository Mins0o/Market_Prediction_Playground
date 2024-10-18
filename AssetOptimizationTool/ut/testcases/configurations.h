#pragma once
#include <string>

namespace config_test {
std::string kInvalidRepetitionConfig = R"({
    "AssetSelection": [
        "asset1",
        "asset2",
        "3asset"
    ],
    "SimulationOptions": {
        "repetitions": "hello",
        "portfolio_mix_strategy": "NeverRebalance"
    },
    "EvaluationOptions": {
        "risk_free_rate": 0.02,
        "expected_return_strategy": "discounted_mean"
    }
})";
std::string kFuzzySimulationStrategyConfig = R"({
    "AssetSelection": [
        "asset1",
        "asset2",
        "3asset"
    ],
    "SimulationOptions": {
        "repetitions": 10000,
        "portfolio_mix_strategy": "never_rebalance"
    },
    "EvaluationOptions": {
        "risk_free_rate": 0.02,
        "expected_return_strategy": "discounted_mean"
    }
})";
}  // namespace config_test