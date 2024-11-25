#pragma once

#include <ctime>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "_interfaces/data_interface.h"
#include "types.h"

namespace asset_optimization_tool::modules {
class DateLine {
  // methods
 public:
  ErrorCode ParseAndAddDate(const std::string& date);
  bool insert(time_t value);
  const std::vector<time_t>& GetDates() const { return dates_; }

 private:
  // data members
 public:
 private:
  std::vector<time_t> dates_;
};

class Asset : public IAsset {
  // methods
 public:
  Asset() = default;
  Asset(const std::string& asset_name,
        std::shared_ptr<const DateLine> date_line_p)
      : asset_name_(asset_name), date_line_p_(date_line_p) {};
  ErrorCode ParseAndAddChangeRate(const std::string& change_rate);
  std::string GetName() const override { return asset_name_; }
  std::vector<double> GetChangeRates() const override { return change_rates_; }
  std::weak_ptr<const DateLine> GetDateLine() const { return date_line_p_; }

 private:
  // data members
 public:
 private:
  std::string asset_name_;
  std::vector<double> change_rates_;
  std::shared_ptr<const DateLine> date_line_p_;
};

class Data : public IData {
  // methods
 public:
  ErrorCode LoadData(const std::string& data_path) override;
  ErrorCode GetAssetNames(std::set<std::string>& asset_names) const override;
  ErrorCode GetAssetDataByNames(
      const std::set<std::string>& asset_names,
      std::vector<const IAsset*>& asset_data) const override;
  const IAsset& operator[](std::string id) const override;

 private:
  ErrorCode ValidateData(const std::string& data_path) const;
  ErrorCode ParseDataFile(const std::string& data_path);
  ErrorCode ParseDataHeaderRow(
      /*I*/ const std::string& first_line,
      /*I*/ std::shared_ptr<const DateLine> date_line_p,
      /*O*/ std::map<std::string, Asset>& assets);
  ErrorCode ParseDataContentRow(
      /*I*/ const std::string& line,
      /*I*/ const std::set<std::string>& added_ids,
      /*O*/ std::shared_ptr<DateLine> date_line_p,
      /*O*/ std::map<std::string, Asset>& assets) const;
  ErrorCode TokenizeLine(const std::string& line,
                         std::vector<std::string>& tokens) const;

 public:
 private:
  // data members
  size_t uuid_tracker_ = 1;
  std::set<std::string> seen_asset_names_;
  std::map<std::string, Asset> assets_;
  std::list<std::shared_ptr<DateLine>> date_lines_;
};
}  // namespace asset_optimization_tool::modules