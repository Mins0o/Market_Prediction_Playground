#pragma once

#include <map>
#include <string>

namespace asset_optimization_tool {

using SimulationId = size_t;
using SimulationOptions = std::map<std::string, std::string>;
using OptimizationOptions = std::map<std::string, std::string>;
using AssetId = size_t;

enum class ErrorCode {
  kSuccess=0,
  kFailure,
  kUnknownError,
  kInvalidFileType,
  kCannotReadFile,
  kFileNotFound,
  kInvalidDelimiter,
};

inline ErrorCode operator&&(ErrorCode lhs, ErrorCode rhs) {
  return (lhs == ErrorCode::kSuccess && rhs == ErrorCode::kSuccess)
             ? ErrorCode::kSuccess
             : ErrorCode::kFailure;
}

inline ErrorCode operator||(ErrorCode lhs, ErrorCode rhs) {
  return (lhs == ErrorCode::kSuccess || rhs == ErrorCode::kSuccess)
             ? ErrorCode::kSuccess
             : ErrorCode::kFailure;
}

}  // namespace asset_optimization_tool