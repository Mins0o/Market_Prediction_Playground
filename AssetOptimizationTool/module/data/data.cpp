#include "data/data.h"

#include <iostream>
#include <string>

namespace asset_optimization_tool::modules {
ErrorCode Data::LoadData(const std::string& data_path) {
  std::cout << "Data::LoadData(" << data_path << ")" << std::endl;
  return ErrorCode::kSuccess;
}
void Data::GetAssetList() { std::cout << "Data::GetAssetList()" << std::endl; }
void Data::GetAssetData() { std::cout << "Data::GetAssetData()" << std::endl; }
}  // namespace asset_optimization_tool::modules