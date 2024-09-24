#pragma once

#include <map>
#include <string>

#include "types.h"

namespace asset_optimization_tool::modules {
class IAsset {
 public:
  virtual ~IAsset() = default;
};

class IData {
 public:
  virtual ~IData() = default;
  virtual ErrorCode LoadData(const std::string& data_path) = 0;
  virtual ErrorCode GetAssetList(
      std::map<AssetId, std::string>& asset_name_list) = 0;
  virtual void GetAssetData() = 0;
};
}  // namespace asset_optimization_tool::modules