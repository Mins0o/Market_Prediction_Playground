#include <memory>
#include <set>
#include <string>

#include "_interfaces/data_interface.h"
#include "asset_optimization_tool.h"  //testing class
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "types.h"

using asset_optimization_tool::AssetOptimizationTool;
using asset_optimization_tool::ErrorCode;

namespace {
std::string kDataPath = "./build/ut/data_example.tsv";
std::string kConfigPath = "./build/ut/config_example.json";
std::set<std::string> kAssetNames = {"asset1", "asset2", "3asset", "4asset"};
}  // namespace

TEST(ToolIntegrationTest, InitializeMethod) {
  std::unique_ptr<AssetOptimizationTool> tool(
      asset_optimization_tool::AssetOptimizationTool::Create());
  EXPECT_EQ(tool->Initialize(kDataPath, kConfigPath), ErrorCode::kSuccess);
}

TEST(ToolIntegrationTest, GetAssetNamesMethod) {
  std::unique_ptr<AssetOptimizationTool> tool(
      asset_optimization_tool::AssetOptimizationTool::Create());
  std::set<std::string> asset_names;
  EXPECT_EQ(tool->Initialize(kDataPath, kConfigPath), ErrorCode::kSuccess);
  EXPECT_EQ(tool->GetAssetNames(asset_names), ErrorCode::kSuccess);
  EXPECT_EQ(asset_names.size(), 4);
  EXPECT_THAT(asset_names, ::testing::UnorderedElementsAreArray(kAssetNames));
}