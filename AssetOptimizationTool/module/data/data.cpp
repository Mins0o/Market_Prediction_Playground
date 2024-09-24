#include "data/data.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace asset_optimization_tool::modules {
ErrorCode Data::LoadData(const std::string& data_path) {
  std::cout << "Data::LoadData()" << std::endl;
  // Check if the data path is valid
  ErrorCode validation_error = ValidateData(data_path);
  // Parse the data file
  ErrorCode parsing_error = ParseDataFile(data_path);

  return validation_error && parsing_error;
}
void Data::GetAssetList() { std::cout << "Data::GetAssetList()" << std::endl; }
void Data::GetAssetData() { std::cout << "Data::GetAssetData()" << std::endl; }

// private methods
ErrorCode Data::ValidateData(const std::string& data_path) const {
  // Check if the data path is one of tsv and csv
  auto extension = data_path.substr(data_path.find_last_of(".") + 1);
  if (extension != "tsv" && extension != "csv") {
    std::cerr << "ValidateData: Invalid file type" << std::endl;
    return ErrorCode::kInvalidFileType;
  }

  std::ifstream data_file(data_path);
  if (!data_file.is_open()) {
    std::cerr << "ValidateData: File not found" << std::endl;
    return ErrorCode::kFileNotFound;
  }

  std::string line;
  if (!std::getline(data_file, line)) {
    std::cerr << "ValidateData: Cannot read file" << std::endl;
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
  DateLine date_line;

  ErrorCode header_parse_error = ParseDataHeader(data_path, date_line, assets);
  // Parse first line for asset names
  // until eof, parse each line:
  // 1. Parse date
  //   1.1

  return ErrorCode::kSuccess;
}

ErrorCode Data::ParseDataHeader(const std::string& first_line,
                                DateLine& date_line,
                                std::vector<Asset>& assets) const {
  std::vector<std::string> tokens;
  TokenizeLine(first_line, tokens);

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