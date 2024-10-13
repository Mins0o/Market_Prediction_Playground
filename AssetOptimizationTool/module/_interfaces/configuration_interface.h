#pragma once

#include <set>
#include <string>

#include "types.h"

namespace asset_optimization_tool::modules {
class IAssetSelections {
 public:
  IAssetSelections() = default;
  virtual ErrorCode AddToSelection(const std::string& asset_name) = 0;
  virtual ErrorCode AddBulkToSelection(
      const std::set<std::string>& asset_names) = 0;
  virtual ErrorCode RemoveFromSelection(const std::string& asset_name) = 0;
  virtual ErrorCode RemoveBulkFromSelection(
      const std::set<std::string>& asset_names) = 0;
  virtual ErrorCode GetSelection(std::set<std::string>& asset_names) const = 0;
  virtual ErrorCode ClearSelection() = 0;
};

class ISimulationConfigurations {
 public:
  ISimulationConfigurations() = default;
  virtual ErrorCode SetRepCount(size_t rep_count) = 0;
};

class IConfiguration {
 public:
  virtual ~IConfiguration() = default;
};

}  // namespace asset_optimization_tool::modules