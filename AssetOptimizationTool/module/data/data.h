#pragma once

#include "data_interface.h"

namespace asset_optimization_tool::modules {
class Data : public DataInterface {
 public:
  void LoadData() override;
  void GetAssetList() override;
  void GetAssetData() override;
};
}  // namespace asset_optimization_tool::modules