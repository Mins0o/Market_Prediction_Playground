#include <memory>
#include <string>

#include "_interfaces/data_interface.h"
#include "_interfaces/module_factory_interface.h"
#include "asset_optimization_tool_impl.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::asset_optimization_tool::AssetOptimizationToolImpl;
using ::asset_optimization_tool::ErrorCode;
using ::asset_optimization_tool::IModuleFactory;
using ::asset_optimization_tool::modules::IData;

namespace {
class MockModuleFactory : public IModuleFactory {
 public:
  MOCK_METHOD(IData*, CreateDataInterface, (), (override));
};
class MockData : public IData {
 public:
  MOCK_METHOD(ErrorCode, LoadData, (const std::string& data_path), (override));
  MOCK_METHOD(void, GetAssetList, (), (override));
  MOCK_METHOD(void, GetAssetData, (), (override));
};
}  // namespace

TEST(ToolLogicTest, InitializeMethod) {
  auto mock_module_factory = std::make_unique<MockModuleFactory>();
  std::unique_ptr mock_data = std::make_unique<MockData>();

  EXPECT_CALL(*mock_module_factory, CreateDataInterface())
      .WillOnce(::testing::Return(mock_data.get()));

  AssetOptimizationToolImpl tool(std::move(mock_module_factory));

  EXPECT_CALL(*mock_data, LoadData("data_path"))
      .WillOnce(::testing::Return(ErrorCode::kSuccess));

  EXPECT_NO_THROW(tool.Initialize("data_path"));
}