#pragma once

#include <ctime>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "_interfaces/data_interface.h"
#include "types.h"

namespace asset_optimization_tool::modules {
class DateLine {
  // methods
 public:
  ErrorCode ParseAndAddDate(const std::string& date);

 private:
  // data members
 public:
 private:
  std::vector<time_t> dates_;
};

class Asset : public IAsset {
  // methods
 public:
  Asset(const std::string& asset_name,
        std::shared_ptr<const DateLine> date_line_p)
      : asset_name_(asset_name), date_line_p_(date_line_p) {};
  ErrorCode ParseAndAddChangeRate(const std::string& change_rate);
  std::string GetName() const { return asset_name_; }

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
  ErrorCode GetAssetList(
      std::map<AssetId, std::string>& asset_name_list) override;
  void GetAssetData() override;

 private:
  ErrorCode ValidateData(const std::string& data_path) const;
  ErrorCode ParseDataFile(const std::string& data_path);
  ErrorCode ParseDataHeaderRow(
      /*I*/ const std::string& first_line,
      /*I*/ std::shared_ptr<const DateLine> date_line_p,
      /*O*/ std::vector<Asset>& assets) const;
  ErrorCode ParseDataContentRow(/*I*/ const std::string& line,
                                /*O*/ std::shared_ptr<DateLine> date_line_p,
                                /*O*/ std::vector<Asset>& assets) const;
  ErrorCode TokenizeLine(const std::string& line,
                         std::vector<std::string>& tokens) const;

 public:
 private:
  // data members
  std::vector<Asset> assets_;
  std::list<std::shared_ptr<DateLine>> date_lines_;
};
}  // namespace asset_optimization_tool::modules