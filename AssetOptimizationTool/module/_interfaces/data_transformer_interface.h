#pragma once

#include <vector>

#include "_interfaces/data_interface.h"
#include "types.h"

namespace asset_optimization_tool::modules {
class IDataTransformer {
 public:
  virtual ErrorCode PrepareData(
      const std::vector<const modules::IAsset *> &data) = 0;
  // Add any additional methods if needed
};
}  // namespace asset_optimization_tool::modules