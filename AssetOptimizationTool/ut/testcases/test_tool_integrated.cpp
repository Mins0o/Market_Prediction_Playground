#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "_interfaces/data_interface.h"
#include "asset_optimization_tool.h"  //testing class
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "types.h"

using asset_optimization_tool::AssetId;
using asset_optimization_tool::AssetOptimizationTool;
using asset_optimization_tool::ErrorCode;

namespace {
std::string kDataPath = "./build/ut/data_example.tsv";
}  // namespace

TEST(ToolIntegrationTest, InitializeMethod) {
  std::unique_ptr<AssetOptimizationTool> tool(
      asset_optimization_tool::AssetOptimizationTool::Create());
  EXPECT_EQ(tool->Initialize(kDataPath), ErrorCode::kSuccess);
}

TEST(ToolIntegrationTest, GetAssetTableMethod) {
  std::unique_ptr<AssetOptimizationTool> tool(
      asset_optimization_tool::AssetOptimizationTool::Create());
  std::map<std::string, AssetId> asset_name_table;
  EXPECT_EQ(tool->Initialize(kDataPath), ErrorCode::kSuccess);
  EXPECT_EQ(tool->GetAssetTable(asset_name_table), ErrorCode::kSuccess);
  EXPECT_EQ(asset_name_table.size(), 4);
}