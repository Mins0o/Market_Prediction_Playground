#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "types.h"

namespace asset_optimization_tool::modules {
class IAsset {
 public:
  virtual ~IAsset() = default;
  virtual std::string GetName() const = 0;
  virtual std::vector<double> GetChangeRates() const = 0;
};

class IData {
 public:
  virtual ~IData() = default;
  virtual ErrorCode LoadData(const std::string& data_path) = 0;
  virtual ErrorCode GetAssetTable(
      std::map<std::string, AssetId>& asset_name_id_table) const = 0;
  virtual ErrorCode GetAssetDataByIds(
      const std::set<AssetId>& asset_ids,
      std::vector<std::unique_ptr<IAsset>>& asset_data) const = 0;
  // indexing operator for the assets with id
  virtual const IAsset& operator[](AssetId id) const = 0;
};
}  // namespace asset_optimization_tool::modules