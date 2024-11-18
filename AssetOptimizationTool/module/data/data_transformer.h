#pragma once

#include <vector>

#include "_interfaces/data_transformer_interface.h"

namespace asset_optimization_tool::modules {

class DataTransformerImpl : public IDataTransformer {
 public:
  ErrorCode PrepareData(
      const std::vector<const modules::IAsset *> &data) override;
  // Add any additional methods if needed
};

}  // namespace asset_optimization_tool