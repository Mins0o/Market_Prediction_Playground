#include "data/data.h"  // testing class
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "types.h"

using ::asset_optimization_tool::AssetId;
using ::asset_optimization_tool::ErrorCode;
using ::asset_optimization_tool::modules::Data;
using ::testing::Return;

namespace {
std::string kDataPath = "./build/ut/data_example.tsv";
std::string kDataPathWrong = "wrong_path.csv";
std::string kDataPathWrongExt = "./build/ut/data_wrong_ext.txt";
std::string kDataPathWrongDelimitter = "./build/ut/data_wrong_delimitter.csv";
}  // namespace

// Example test case for the Analyzer class
TEST(DataTest, LoadMethod) {
  Data data;

  ASSERT_EQ(data.LoadData(::kDataPath), ErrorCode::kSuccess);
}

TEST(DataTest, LoadNonExistentFile) {
  Data data;

  ASSERT_EQ(data.LoadData(kDataPathWrong), ErrorCode::kFileNotFound);
}

TEST(DataTest, LoadWrongFileType) {
  Data data;

  ASSERT_EQ(data.LoadData(::kDataPathWrongExt), ErrorCode::kInvalidFileType);
}

TEST(DataTest, LoadWrongDelimitter) {
  Data data;

  ASSERT_EQ(data.LoadData(::kDataPathWrongDelimitter),
            ErrorCode::kInvalidDelimiter);
}

TEST(DataTest, GetAssetListMethod) {
  Data data;

  std::map<AssetId, std::string> asset_name_list;

  ASSERT_EQ(data.LoadData(::kDataPath), ErrorCode::kSuccess);
  ASSERT_EQ(data.GetAssetList(asset_name_list), ErrorCode::kSuccess);
  ASSERT_EQ(asset_name_list.size(), 4);
  ASSERT_EQ(asset_name_list[0], "asset1");
  ASSERT_EQ(asset_name_list[1], "asset2");
  ASSERT_EQ(asset_name_list[2], "3asset");
  ASSERT_EQ(asset_name_list[3], "4asset");
}
