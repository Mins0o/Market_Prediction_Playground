#include <map>
#include <memory>
#include <string>

#include "_interfaces/data_interface.h"
#include "_interfaces/module_factory_interface.h"
#include "asset_optimization_tool_impl.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "types.h"

using ::asset_optimization_tool::AssetId;
using ::asset_optimization_tool::AssetOptimizationToolImpl;
using ::asset_optimization_tool::ErrorCode;
using ::asset_optimization_tool::IModuleFactory;
using ::asset_optimization_tool::modules::IData;

namespace {
class MockData : public IData {
 public:
  MOCK_METHOD(ErrorCode, LoadData, (const std::string& data_path), (override));
  MOCK_METHOD(ErrorCode, GetAssetList,
              ((std::map<AssetId, std::string> & asset_name_list)), (override));
  MOCK_METHOD(void, GetAssetData, (), (override));
};
}  // namespace

TEST(ToolLogicTest, InitializeMethod) {
  MockData* mock_data_p = new MockData();

  AssetOptimizationToolImpl tool(mock_data_p);

  EXPECT_CALL(*mock_data_p, LoadData("data_path"))
      .WillOnce(::testing::Return(ErrorCode::kSuccess));

  EXPECT_NO_THROW(tool.Initialize("data_path"));
}