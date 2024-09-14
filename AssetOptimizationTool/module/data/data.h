#pragma once

#include <string>

#include "_interfaces/data_interface.h"
#include "types.h"

namespace asset_optimization_tool::modules {
class Data : public IData {
 public:
  ErrorCode LoadData(const std::string& data_path) override;
  void GetAssetList() override;
  void GetAssetData() override;
};
}  // namespace asset_optimization_tool::modules