#include "configuration/configuration.h"

#include <json/json.h>

#include <fstream>
#include <iostream>

namespace asset_optimization_tool::modules {
// ------------------- SimulationConfigurations -------------------

// ------------------- EvaluationConfigurations -------------------

// ------------------- Configuration -------------------
ErrorCode Configuration::LoadConfiguration(const std::string& config_path) {
  std::ifstream config_file(config_path);
  if (!config_file.is_open()) {
    std::cerr << "Configuration::LoadConfiguration: File not found - "
              << config_path << std::endl;
    return ErrorCode::kFileNotFound;
  }

  return ErrorCode::kSuccess;
}
}  // namespace asset_optimization_tool::modules