#include "data/data.h"

#include <ctime>
#include <fstream>
#include <iostream>
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
ErrorCode Data::GetAssetList(std::map<AssetId, std::string>& asset_name_list) {
  asset_name_list.clear();
  for (size_t i = 0; i < assets_.size(); ++i) {
    asset_name_list.emplace(i, assets_[i].GetName());
  }
  return ErrorCode::kSuccess;
}

void Data::GetAssetData() { std::cout << "Data::GetAssetData()" << std::endl; }

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

  std::vector<Asset> assets;
  auto date_line = std::make_shared<DateLine>();

  std::string first_line;
  std::getline(data_file, first_line);
  ErrorCode header_parse_error =
      ParseDataHeaderRow(first_line, date_line, assets);
  if (header_parse_error != ErrorCode::kSuccess) {
    std::cerr << "ParseDataFile: Error parsing header" << std::endl;
    return header_parse_error;
  }

  for (std::string line; std::getline(data_file, line);) {
    ErrorCode content_parse_error =
        ParseDataContentRow(line, date_line, assets);
    if (content_parse_error != ErrorCode::kSuccess) {
      std::cerr << "ParseDataFile: Error parsing content" << std::endl;
      return content_parse_error;
    }
  }

  data_file.close();
  date_lines_.emplace_back(std::move(date_line));
  assets_.insert(assets_.end(), std::make_move_iterator(assets.begin()),
                 std::make_move_iterator(assets.end()));

  return ErrorCode::kSuccess;
}

ErrorCode Data::ParseDataHeaderRow(const std::string& first_line,
                                   std::shared_ptr<const DateLine> date_line,
                                   std::vector<Asset>& assets) const {
  std::vector<std::string> tokens;
  TokenizeLine(first_line, tokens);

  tokens.erase(tokens.begin());  // Remove the first token which is "Date"
  for (const auto& token : tokens) {
    assets.emplace_back(Asset(token, date_line));
  }

  return ErrorCode::kSuccess;
}

ErrorCode Data::ParseDataContentRow(const std::string& line,
                                    std::shared_ptr<DateLine> date_line,
                                    std::vector<Asset>& assets) const {
  std::vector<std::string> tokens;
  TokenizeLine(line, tokens);

  ErrorCode date_parse_error = date_line->ParseAndAddDate(tokens[0]);
  if (date_parse_error != ErrorCode::kSuccess) {
    std::cerr << "ParseDataFile: Error parsing date - " << tokens[0]
              << std::endl;
    return date_parse_error;
  }

  for (size_t i = 1; i < tokens.size(); ++i) {
    ErrorCode change_rate_parse_error =
        assets[i - 1].ParseAndAddChangeRate(tokens[i]);
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
  return ErrorCode::kSuccess;
}

}  // namespace asset_optimization_tool::modules