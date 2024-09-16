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

namespace {}  // namespace

TEST(ToolInterfaceTest, InitializeMethod) {
  std::unique_ptr<AssetOptimizationTool> tool(
      asset_optimization_tool::AssetOptimizationTool::Create());
  EXPECT_NO_THROW(tool->Initialize("data_path"));
}