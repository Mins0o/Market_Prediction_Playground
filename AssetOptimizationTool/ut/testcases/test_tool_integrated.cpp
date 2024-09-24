#include <map>
#include <memory>
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
std::string kDataPath = "./build/ut/data_example.tsv";
}  // namespace

TEST(ToolIntegrationTest, InitializeMethod) {
  std::unique_ptr<AssetOptimizationTool> tool(
      asset_optimization_tool::AssetOptimizationTool::Create());
  EXPECT_NO_THROW(tool->Initialize(kDataPath));
}