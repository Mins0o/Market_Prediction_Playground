#include "_interfaces/data_interface.h"
#include "data/data.h"  // testing class
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "types.h"

using ::asset_optimization_tool::ErrorCode;
using ::asset_optimization_tool::modules::Data;
using ::asset_optimization_tool::modules::IAsset;
using ::testing::Return;

namespace {
std::string kDataPath = "./build/ut/data_example.tsv";
std::string kDataPathWrong = "wrong_path.csv";
std::string kDataPathWrongExt = "./build/ut/data_wrong_ext.txt";
std::string kDataPathWrongDelimitter = "./build/ut/data_wrong_delimitter.csv";
std::set<std::string> kAssetNames = {"asset1", "asset2", "3asset", "4asset"};
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

TEST(DataTest, GetAssetNamesMethod) {
  Data data;

  std::set<std::string> asset_names;

  ASSERT_EQ(data.LoadData(::kDataPath), ErrorCode::kSuccess);
  ASSERT_EQ(data.GetAssetNames(asset_names), ErrorCode::kSuccess);
  ASSERT_EQ(asset_names.size(), 4);
  ASSERT_THAT(asset_names, ::testing::UnorderedElementsAreArray(kAssetNames));
}

TEST(DataTest, GetAssetDataByIdsMethod) {
  Data data;

  std::vector<const IAsset*> asset_data;

  ASSERT_EQ(data.LoadData(::kDataPath), ErrorCode::kSuccess);
  ASSERT_EQ(data.GetAssetDataByNames(kAssetNames, asset_data),
            ErrorCode::kSuccess);
  ASSERT_EQ(asset_data.size(), 4);
  for (const auto& asset : asset_data) {
    ASSERT_THAT(kAssetNames, ::testing::Contains(asset->GetName()));
    std::cout << asset->GetName() << "\t";
    for (const auto& rate : asset->GetChangeRates()) {
      std::cout << rate << "\t";
    }
    std::cout << std::endl;
  }
}