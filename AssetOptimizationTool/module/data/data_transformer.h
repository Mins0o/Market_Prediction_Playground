#pragma once

#include <vector>

#include "_interfaces/data_transformer_interface.h"
#include "data.h"

namespace asset_optimization_tool::modules {

class DataTransformer : public IDataTransformer {
 public:
  ErrorCode PrepareData(
      const std::vector<const IAsset *> &data,
      std::vector<std::unique_ptr<IAsset>> &prepared_data) override;

 private:
  ErrorCode UnionDateLines(const std::vector<const Asset *> &data,
                           DateLine &date_line_union) const;
  ErrorCode CloneData(const std::vector<const Asset *> &data,
                      std::vector<std::unique_ptr<Asset>> &prepared_data) const;
  ErrorCode FillMissingData(
      const DateLine &date_line_union,
      std::vector<std::unique_ptr<Asset>> &prepared_data) const;
  ErrorCode GetStartDate(const Asset &asset, time_t &start_date) const;
  ErrorCode GetEndDate(const Asset &asset, time_t &end_date) const;
};

}  // namespace asset_optimization_tool::modules