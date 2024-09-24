#pragma once

#include <ctime>
#include <set>
#include <string>
#include <vector>

#include "_interfaces/data_interface.h"
#include "types.h"

namespace asset_optimization_tool::modules {
class DateLine {
  // methods
 public:
 private:
  // data members
 public:
 private:
  std::vector<time_t> dates_;
};

class Asset : public IAsset {
  // methods
 public:
 private:
  // data members
 public:
 private:
  std::string asset_name_;
  std::vector<double> change_rates_;
  const DateLine& date_line_ref_;
};

class Data : public IData {
  // methods
 public:
  ErrorCode LoadData(const std::string& data_path) override;
  void GetAssetList() override;
  void GetAssetData() override;

 private:
  ErrorCode ValidateData(const std::string& data_path) const;
  ErrorCode ParseDataFile(const std::string& data_path);
  ErrorCode ParseDataHeader(/*I*/ const std::string& first_line,
                            /*O*/ DateLine& date_line,
                            /*O*/ std::vector<Asset>& assets) const;
  ErrorCode TokenizeLine(const std::string& line,
                         std::vector<std::string>& tokens) const;

 public:
 private:
  // data members
  std::vector<Asset> assets_;
  std::set<DateLine> date_line_;
};
}  // namespace asset_optimization_tool::modules