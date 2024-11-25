#pragma once

#include <memory>
#include <vector>

#include "_interfaces/data_interface.h"
#include "types.h"

namespace asset_optimization_tool::modules {
class IDataTransformer {
 public:
  virtual ErrorCode PrepareData(
      const std::vector<const modules::IAsset *> &data,
      std::vector<std::unique_ptr<modules::IAsset>> &prepared_data) = 0;
};
}  // namespace asset_optimization_tool::modules