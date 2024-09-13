#pragma once

namespace asset_optimization_tool::modules {
class DataInterface {
 public:
  virtual void LoadData() = 0;
  virtual void GetAssetList() = 0;
  virtual void GetAssetData() = 0;
};
}  // namespace asset_optimization_tool::modules