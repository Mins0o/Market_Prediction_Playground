#pragma once

#include <set>
#include <string>

#include "types.h"

namespace asset_optimization_tool::modules {
class IConfiguration {
 public:
  virtual ~IConfiguration() = default;
  virtual ErrorCode LoadConfiguration(const std::string& config_path) = 0;
  virtual ErrorCode GetConfigurationKeys(std::set<std::string>& keys) const = 0;
  virtual ErrorCode GetConfigurationValue(const std::string& key,
                                          std::string& value) const = 0;

  virtual ErrorCode SetAssetSelection(
      const std::set<std::string>& asset_names) = 0;
  virtual ErrorCode SetSimulationOption(const std::string& key,
                                        const std::string& value) = 0;
  virtual ErrorCode SetOptimizationOption(const std::string& key,
                                          const std::string& value) = 0;
};
}  // namespace asset_optimization_tool::modules