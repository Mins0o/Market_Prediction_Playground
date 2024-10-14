#include "data/data.h"

#include <fstream>
#include <iostream>
#include <rapidfuzz/fuzz.hpp>
#include <sstream>

namespace {
std::vector<std::pair<std::string, double>> extract(
    const std::string& query, const std::set<std::string>& choices,
    const double score_cutoff = 0.0) {
  std::vector<std::pair<std::string, double>> results;
  rapidfuzz::fuzz::CachedRatio<char> scorer(query);

  for (const auto& choice : choices) {
    double score = scorer.similarity(choice, score_cutoff);
    if (score >= score_cutoff) {
      results.emplace_back(choice, score);
    }
  }
  return results;
}
}  // namespace

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
ErrorCode Data::GetAssetNames(std::set<std::string>& asset_names) const {
  for (const auto& [asset_name, asset] : assets_) {
    asset_names.insert(asset_name);
  }
  return ErrorCode::kSuccess;
}

ErrorCode Data::GetAssetDataByNames(
    const std::set<std::string>& asset_names,
    std::vector<const IAsset*>& asset_data) const {
  asset_data.clear();
  std::set<std::string> current_asset_names;
  for (const auto& [asset_name, asset] : assets_) {
    current_asset_names.insert(asset_name);
  }
  for (const auto& asset_name : asset_names) {
    auto search_results = ::extract(asset_name, current_asset_names, 80);
    if (search_results.empty()) {
      std::cerr << "Data::GetAssetDataByNames: No search results" << std::endl;
      std::cerr << "Did you mean:" << std::endl;
      for (const auto& candidate :
           ::extract(asset_name, current_asset_names, 49)) {
        std::cerr << candidate.first << std::endl;
      }
      continue;
    }
    double max_score = 0;
    std::string match;
    for (const auto& result : search_results) {
      if (result.second > max_score) {
        match = result.first;
        max_score = result.second;
      }
    }
    std::cout << "Matched asset name for " << asset_name << ": " << match
              << std::endl;
    asset_data.emplace_back(&assets_.at(match));
  }
  return ErrorCode::kSuccess;
}

const IAsset& Data::operator[](std::string id) const {
  const auto& asset = assets_.find(id);
  if (asset == assets_.end()) {
    std::cerr << "Data::operator[]: Invalid asset id - " << id << std::endl;
    throw ErrorCode::kAssetNotFound;
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
    return ErrorCode::kCannotReadFile;
  }

  std::string line;
  if (!std::getline(data_file, line)) {
    std::cerr << "ValidateData: Cannot read file - " << data_path << std::endl;
    data_file.close();
    return ErrorCode::kCannotReadFile;
  }
  data_file.close();

  // For now, only tab delimitted is supported.
  if (line.find('\t') == std::string::npos) {
    std::cerr << "ValidateData: Invalid delimiter. Only tabs are supported."
              << std::endl;
    return ErrorCode::kInvalidDelimiter;
  }
  return ErrorCode::kSuccess;
}

ErrorCode Data::ParseDataFile(const std::string& data_path) {
  std::ifstream data_file(data_path);
  if (!data_file.is_open()) {
    return ErrorCode::kCannotReadFile;
  }

  std::map<std::string, Asset> assets;
  std::set<std::string> added_ids;
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
                                   std::map<std::string, Asset>& assets) {
  std::vector<std::string> tokens;
  TokenizeLine(first_line, tokens);

  tokens.erase(tokens.begin());  // Remove the first token which is "Date"
  for (const auto& token : tokens) {
    auto asset_name = token;
    if (seen_asset_names_.find(token) != seen_asset_names_.end()) {
      asset_name += "_" + std::to_string(uuid_tracker_++);
    } else {
      seen_asset_names_.insert(token);
    }
    assets[asset_name] = Asset(std::move(asset_name), date_line);
  }
  return ErrorCode::kSuccess;
}

ErrorCode Data::ParseDataContentRow(
    const std::string& line, const std::set<std::string>& added_ids,
    std::shared_ptr<DateLine> date_line,
    std::map<std::string, Asset>& assets) const {
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