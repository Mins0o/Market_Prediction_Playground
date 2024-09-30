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
using ::asset_optimization_tool::modules::IAsset;
using ::asset_optimization_tool::modules::IData;

namespace {
class MockData : public IData {
 public:
  MOCK_METHOD(ErrorCode, LoadData, (const std::string& data_path), (override));
  MOCK_METHOD(ErrorCode, GetAssetTable,
              ((std::map<std::string, AssetId> & asset_name_table)),
              (const, override));
  MOCK_METHOD(ErrorCode, GetAssetDataByIds,
              ((const std::set<AssetId>& asset_ids),
               (std::vector<std::unique_ptr<IAsset>> & asset_data)),
              (const, override));

  const IAsset& operator[](AssetId id) const override { return asset_; }

 private:
  class TestAsset : public IAsset {
   public:
    TestAsset() = default;
    std::string GetName() const override { return ""; }
    std::vector<double> GetChangeRates() const override { return {}; }
  } asset_;
  std::set<std::string> seen_asset_names_;
  AssetId uuid_tracker_ = 0;
};
}  // namespace

TEST(ToolLogicTest, InitializeMethod) {
  MockData* mock_data_p = new MockData();

  AssetOptimizationToolImpl tool(mock_data_p);

  EXPECT_CALL(*mock_data_p, LoadData("data_path"))
      .WillOnce(::testing::Return(ErrorCode::kSuccess));

  EXPECT_NO_THROW(tool.Initialize("data_path"));
}