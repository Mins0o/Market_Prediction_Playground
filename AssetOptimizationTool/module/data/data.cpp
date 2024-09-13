#include "data.h"

#include <iostream>

namespace asset_optimization_tool::modules {
void Data::LoadData() { std::cout << "Data::LoadData()" << std::endl; }
void Data::GetAssetList() { std::cout << "Data::GetAssetList()" << std::endl; }
void Data::GetAssetData() { std::cout << "Data::GetAssetData()" << std::endl; }
}  // namespace asset_optimization_tool::modules