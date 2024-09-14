#include <map>
#include <set>
#include <string>
#include <vector>

#include "asset_optimization_tool.h"  //testing class
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "types.h"

using asset_optimization_tool::AssetOptimizationTool;
using asset_optimization_tool::ErrorCode;

namespace {
class MockAssetOptimizationTool
    : public asset_optimization_tool::AssetOptimizationTool {
 public:
  MOCK_METHOD(ErrorCode, Initialize, (const std::string& data_path),
              (override));
  MOCK_METHOD(ErrorCode, GetAssetList,
              ((std::map<asset_optimization_tool::AssetId, std::string> &
                asset_list)),
              (override));
  MOCK_METHOD(ErrorCode, SelectAssets,
              ((const std::set<asset_optimization_tool::AssetId>& asset_ids)),
              (override));
  MOCK_METHOD(ErrorCode, SetSimulationOption,
              (const std::string& key, const std::string& value), (override));
  MOCK_METHOD(ErrorCode, Simulate,
              (size_t num_simulations, asset_optimization_tool::SimulationId),
              (override));
  MOCK_METHOD(ErrorCode, Simulate,
              (const asset_optimization_tool::SimulationOptions& options,
               asset_optimization_tool::SimulationId),
              (override));
  MOCK_METHOD(ErrorCode, SetOptimizationOption,
              (const std::string& key, const std::string& value), (override));
  MOCK_METHOD(ErrorCode, Optimize,
              (asset_optimization_tool::SimulationId simulation_id),
              (override));
  MOCK_METHOD(ErrorCode, Optimize, (), (override));
  MOCK_METHOD(
      ErrorCode, Test,
      ((const std::vector<asset_optimization_tool::AssetId>& asset_ids),
       (const std::vector<double>& weights),
       const asset_optimization_tool::OptimizationOptions& evaluation_method),
      (override));
  MOCK_METHOD(
      ErrorCode, Evaluate,
      ((const std::vector<double>& simulation_result),
       const asset_optimization_tool::OptimizationOptions& evaluation_method,
       double result),
      (override));
};
}  // namespace

std::unique_ptr<AssetOptimizationTool>
asset_optimization_tool::AssetOptimizationTool::Create() {
  return std::make_unique<MockAssetOptimizationTool>();
}
TEST(ToolInterfaceTest, InitializeMethod) {
  MockAssetOptimizationTool mock_asset_optimization_tool;
  EXPECT_CALL(mock_asset_optimization_tool, Initialize("data_path"))
      .WillOnce(testing::Return(ErrorCode::kSuccess));
  EXPECT_NO_THROW(mock_asset_optimization_tool.Initialize("data_path"));
}