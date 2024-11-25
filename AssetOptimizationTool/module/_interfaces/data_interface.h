#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "types.h"

namespace asset_optimization_tool::modules {
class IAsset {
 public:
  IAsset() = default;
  virtual ~IAsset() = default;
  virtual std::string GetName() const = 0;
  virtual std::vector<double> GetChangeRates() const = 0;
};

class IData {
 public:
  virtual ~IData() = default;
  virtual ErrorCode LoadData(const std::string& data_path) = 0;
  virtual ErrorCode GetAssetNames(std::set<std::string>& asset_names) const = 0;
  virtual ErrorCode GetAssetDataByNames(
      const std::set<std::string>& asset_names,
      std::vector<const IAsset*>& asset_data) const = 0;
  // indexing operator for the assets with asset name
  virtual const IAsset& operator[](std::string asset_name) const = 0;
};
}  // namespace asset_optimization_tool::modules