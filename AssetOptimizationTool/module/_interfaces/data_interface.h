#pragma once

#include <string>

#include "types.h"

namespace asset_optimization_tool::modules {
class IData {
 public:
  virtual ~IData() = default;
  virtual ErrorCode LoadData(const std::string& data_path) = 0;
  virtual void GetAssetList() = 0;
  virtual void GetAssetData() = 0;
};
}  // namespace asset_optimization_tool::modules