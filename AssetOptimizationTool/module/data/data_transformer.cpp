#include "data_transformer.h"

#include <iostream>
#include <set>

#include "types.h"

namespace asset_optimization_tool::modules {

ErrorCode DataTransformer::PrepareData(
    const std::vector<const IAsset *> &data,
    std::vector<std::unique_ptr<IAsset>> &prepared_data) {
  // treat data as a vector of assets
  if (data.empty()) {
    return ErrorCode::kInvalidData;
  }
  std::vector<const Asset *> assets;
  std::vector<std::unique_ptr<Asset>> prepared_assets;
  std::cout << "DataTransformer::PrepareData" << std::endl;
  for (const auto &asset_i : data) {
    const Asset *asset = dynamic_cast<const Asset *>(asset_i);
    if (asset == nullptr) {
      return ErrorCode::kInvalidData;
    }
    assets.push_back(asset);
  }
  DateLine date_line_union_;
  if (auto err = UnionDateLines(assets, date_line_union_);
      err != ErrorCode::kSuccess) {
    return err;
  }
  std::cout << "DataTransformer::PrepareData: UnionDateLines" << std::endl;
  prepared_data.clear();
  if (auto err = CloneData(assets, prepared_assets);
      err != ErrorCode::kSuccess) {
    return err;
  }

  std::cout << "DataTransformer::PrepareData: CloneData" << std::endl;
  if (auto err = FillMissingData(date_line_union_, prepared_assets);
      err != ErrorCode::kSuccess) {
    return err;
  }

  // now we should align the start and end dates of the assets.
  // This needs some decision input or config from the user.
  for (auto &asset : prepared_assets) {
    prepared_data.push_back(std::move(asset));
  }
  return ErrorCode::kSuccess;
}

/**
 * @brief Union all date lines of the assets.
 * For each asset
 * 1. Get the date line
 * 2. Check if the date line is already seen
 * 3. If not, add all dates to the union date line
 */
ErrorCode DataTransformer::UnionDateLines(
    const std::vector<const modules::Asset *> &data,
    DateLine &date_line_union) const {
  std::set<const DateLine *> seen_date_line;
  for (const auto &asset : data) {
    if (asset == nullptr) {
      return ErrorCode::kInvalidData;
    }
    const auto date_line_p = asset->GetDateLine().lock();
    if (date_line_p == nullptr) {
      return ErrorCode::kInvalidData;
    }
    if (seen_date_line.find(date_line_p.get()) != seen_date_line.end()) {
      continue;
    }
    seen_date_line.insert(date_line_p.get());
    for (const auto &date : date_line_p->GetDates()) {
      date_line_union.insert(date);
    }
  }
  return ErrorCode::kSuccess;
}

ErrorCode DataTransformer::CloneData(
    const std::vector<const modules::Asset *> &data,
    std::vector<std::unique_ptr<modules::Asset>> &prepared_data) const {
  for (const auto &asset_i : data) {
    const Asset *asset = dynamic_cast<const Asset *>(asset_i);
    if (asset == nullptr) {
      return ErrorCode::kInvalidData;
    }
    prepared_data.push_back(std::make_unique<Asset>(*asset));
  }
  return ErrorCode::kSuccess;
}

ErrorCode DataTransformer::FillMissingData(
    const DateLine &date_line_union,
    std::vector<std::unique_ptr<modules::Asset>> &prepared_data) const {
  // TODO
  // The final date line will be a union of all date lines.
  // If for an asset, there is no data for a date,
  // it should be filled with a 0, since it means no change.
  return ErrorCode::kSuccess;
}

ErrorCode DataTransformer::GetStartDate(const Asset &asset,
                                        time_t &start_date) const {
  // Start date is the first date when the value of the change is not 0.
  // If all values are 0, then the start date is the first date.
  const auto &change_rates = asset.GetChangeRates();
  const auto &date_line = asset.GetDateLine().lock();
  if (date_line == nullptr) {
    return ErrorCode::kInvalidData;
  }
  const auto &dates = date_line->GetDates();
  for (size_t i = 0; i < change_rates.size(); ++i) {
    if (change_rates[i] != 0) {
      start_date = dates[i];
      return ErrorCode::kSuccess;
    }
  }
}

ErrorCode DataTransformer::GetEndDate(const Asset &asset,
                                      time_t &end_date) const {
  // End date is the last date when the value of the change is not 0.
  // If all values are 0, then the end date is the last date.
  const auto &change_rates = asset.GetChangeRates();
  const auto &date_line = asset.GetDateLine().lock();
  if (date_line == nullptr) {
    return ErrorCode::kInvalidData;
  }
  const auto &dates = date_line->GetDates();
  for (size_t i = change_rates.size() - 1; i >= 0; --i) {
    if (change_rates[i] != 0) {
      end_date = dates[i];
      return ErrorCode::kSuccess;
    }
  }
}

}  // namespace asset_optimization_tool::modules
