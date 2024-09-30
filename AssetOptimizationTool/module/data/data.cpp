#include "data/data.h"

#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace asset_optimization_tool::modules {
// ------------------- DateLine -------------------
ErrorCode DateLine::ParseAndAddDate(const std::string& date_string) {
  std::tm date;
  strptime(date_string.c_str(), "%Y-%m-%d", &date);
  dates_.emplace_back(std::mktime(&date));
  return ErrorCode::kSuccess;
}
// ------------------- Asset -------------------
ErrorCode Asset::ParseAndAddChangeRate(const std::string& change_rate) {
  if (change_rate == "NA" || change_rate == "") {
    change_rates_.emplace_back(0);
    return ErrorCode::kSuccess;
  }
  try {
    change_rates_.emplace_back(std::stod(change_rate));
  } catch (const std::invalid_argument& e) {
    std::cerr << "Asset::ParseAndAddChangeRate: Invalid string - "
              << change_rate << std::endl;
    return ErrorCode::kInvalidArgument;
  }
  return ErrorCode::kSuccess;
}
// ------------------- Data -------------------
ErrorCode Data::LoadData(const std::string& data_path) {
  ErrorCode validation_error = ValidateData(data_path);
  if (validation_error != ErrorCode::kSuccess) {
    std::cerr << "Data::LoadData: Validation error" << std::endl;
    return validation_error;
  }
  ErrorCode parsing_error = ParseDataFile(data_path);
  if (parsing_error != ErrorCode::kSuccess) {
    std::cerr << "Data::LoadData: Parsing error" << std::endl;
    return parsing_error;
  }

  return ErrorCode::kSuccess;
}
ErrorCode Data::GetAssetTable(
    std::map<std::string, AssetId>& asset_name_id_table) const {
  asset_name_id_table.clear();
  bool error_encountered = false;
  for (const auto& asset : assets_) {
    const auto& name = asset.second.GetName();
    asset_name_id_table[name] = asset.first;
  }
  if (error_encountered) {
    return ErrorCode::kInvalidData;
  }
  return ErrorCode::kSuccess;
}

ErrorCode Data::GetAssetDataByIds(
    const std::set<AssetId>& asset_ids,
    std::vector<std::unique_ptr<IAsset>>& asset_data) const {
  asset_data.clear();
  bool error_encountered = false;
  for (const auto& asset_id : asset_ids) {
    const auto& asset = assets_.find(asset_id);
    if (asset == assets_.end()) {
      std::cerr << "Data::GetAssetDataByIds: Invalid asset id - " << asset_id
                << std::endl;
      error_encountered = true;
      continue;
    }
    asset_data.emplace_back(std::make_unique<Asset>(asset->second));
  }
  if (error_encountered) {
    return ErrorCode::kInvalidAssetId;
  }
  return ErrorCode::kSuccess;
}

const IAsset& Data::operator[](AssetId id) const {
  const auto& asset = assets_.find(id);
  if (asset == assets_.end()) {
    std::cerr << "Data::operator[]: Invalid asset id - " << id << std::endl;
    throw ErrorCode::kInvalidAssetId;
  }
  return asset->second;
}

// private methods
ErrorCode Data::ValidateData(const std::string& data_path) const {
  auto extension = data_path.substr(data_path.find_last_of(".") + 1);
  if (extension != "tsv" && extension != "csv") {
    std::cerr << "ValidateData: Invalid file type - " << data_path << std::endl;
    return ErrorCode::kInvalidFileType;
  }

  std::ifstream data_file(data_path);
  if (!data_file.is_open()) {
    std::cerr << "ValidateData: File not found - " << data_path << std::endl;
    return ErrorCode::kFileNotFound;
  }

  std::string line;
  if (!std::getline(data_file, line)) {
    std::cerr << "ValidateData: Cannot read file - " << data_path << std::endl;
    return ErrorCode::kCannotReadFile;
  }

  // For now, only tab delimitted is supported.
  if (line.find('\t') == std::string::npos) {
    std::cerr << "ValidateData: Invalid delimiter. Only tabs are supported."
              << std::endl;
    return ErrorCode::kInvalidDelimiter;
  }
  data_file.close();
  return ErrorCode::kSuccess;
}

ErrorCode Data::ParseDataFile(const std::string& data_path) {
  std::ifstream data_file(data_path);
  if (!data_file.is_open()) {
    return ErrorCode::kFileNotFound;
  }

  std::map<AssetId, Asset> assets;
  std::set<AssetId> added_ids;
  auto date_line = std::make_shared<DateLine>();

  std::string first_line;
  std::getline(data_file, first_line);
  ErrorCode header_parse_error =
      ParseDataHeaderRow(first_line, date_line, assets);
  if (header_parse_error != ErrorCode::kSuccess) {
    std::cerr << "ParseDataFile: Error parsing header" << std::endl;
    return header_parse_error;
  }
  // added_ids are collection of first item from assets
  for (const auto& [id, asset] : assets) {
    added_ids.insert(id);
  }

  for (std::string line; std::getline(data_file, line);) {
    ErrorCode content_parse_error =
        ParseDataContentRow(line, added_ids, date_line, assets);
    if (content_parse_error != ErrorCode::kSuccess) {
      std::cerr << "ParseDataFile: Error parsing content" << std::endl;
      return content_parse_error;
    }
  }

  data_file.close();
  date_lines_.emplace_back(std::move(date_line));
  assets_.insert(assets.begin(), assets.end());

  return ErrorCode::kSuccess;
}

ErrorCode Data::ParseDataHeaderRow(const std::string& first_line,
                                   std::shared_ptr<const DateLine> date_line,
                                   std::map<AssetId, Asset>& assets) {
  std::vector<std::string> tokens;
  TokenizeLine(first_line, tokens);

  tokens.erase(tokens.begin());  // Remove the first token which is "Date"
  for (const auto& token : tokens) {
    auto asset_name = token;
    if (seen_asset_names_.find(token) != seen_asset_names_.end()) {
      asset_name += "_" + std::to_string(uuid_tracker_);
    } else {
      seen_asset_names_.insert(token);
    }
    assets[uuid_tracker_++] = Asset(std::move(asset_name), date_line);
  }
  return ErrorCode::kSuccess;
}

ErrorCode Data::ParseDataContentRow(const std::string& line,
                                    const std::set<AssetId>& added_ids,
                                    std::shared_ptr<DateLine> date_line,
                                    std::map<AssetId, Asset>& assets) const {
  std::vector<std::string> tokens;
  TokenizeLine(line, tokens);

  ErrorCode date_parse_error = date_line->ParseAndAddDate(tokens[0]);
  if (date_parse_error != ErrorCode::kSuccess) {
    std::cerr << "ParseDataFile: Error parsing date - " << tokens[0]
              << std::endl;
    return date_parse_error;
  }
  auto it = added_ids.begin();
  for (size_t i = 0; i < added_ids.size(); ++i, ++it) {
    ErrorCode change_rate_parse_error =
        assets[*it].ParseAndAddChangeRate(tokens[i + 1]);
    if (change_rate_parse_error != ErrorCode::kSuccess) {
      std::cerr << "ParseDataFile: Error parsing change rate" << std::endl;
      return change_rate_parse_error;
    }
  }
  return ErrorCode::kSuccess;
}

ErrorCode Data::TokenizeLine(const std::string& line,
                             std::vector<std::string>& tokens) const {
  std::stringstream ss(line);
  std::string token;
  while (std::getline(ss, token, '\t')) {
    tokens.emplace_back(std::move(token));
  }
  if (!line.empty() && line.back() == '\t') {
    tokens.emplace_back("");
  }
  return ErrorCode::kSuccess;
}

}  // namespace asset_optimization_tool::modules