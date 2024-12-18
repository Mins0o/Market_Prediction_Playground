#include "_interfaces/data_interface.h"
#include "_interfaces/data_transformer_interface.h"
#include "data/data.h"              // testing class
#include "data/data_transformer.h"  //testing class
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "types.h"

using ::asset_optimization_tool::ErrorCode;
using ::asset_optimization_tool::modules::Data;
using ::asset_optimization_tool::modules::DataTransformer;
using ::asset_optimization_tool::modules::IAsset;
using ::testing::Return;

namespace {
std::string kDataPath = "./build/ut/data_example.tsv";
std::string kDataPathWrong = "wrong_path.csv";
std::string kDataPathWrongExt = "./build/ut/data_wrong_ext.txt";
std::string kDataPathWrongDelimitter = "./build/ut/data_wrong_delimitter.csv";
std::set<std::string> kAssetNames = {"asset1", "asset2", "3asset", "4asset"};
std::set<std::string> kAssetNameQuery = {"set1", "ast2", "3ast", "4set"};
std::map<std::string, std::vector<double>> kAssetData = {
    {"asset1", {0, 0, 0, 0, 0.3}},
    {"asset2", {0, 0, 0.1, -0.5, 0}},
    {"3asset", {0, 0, 0, -1, 0}},
    {"4asset", {0, 0, 0, 2, 0.2}}};
}  // namespace

// Example test case for the Analyzer class
TEST(DataTest, LoadMethod) {
  Data data;

  ASSERT_EQ(data.LoadData(::kDataPath), ErrorCode::kSuccess);
}

TEST(DataTest, LoadNonExistentFile) {
  Data data;

  ASSERT_EQ(data.LoadData(kDataPathWrong), ErrorCode::kCannotReadFile);
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

TEST(DataTest, GetAssetDataByNamesMethod) {
  Data data;

  std::vector<const IAsset *> asset_data;

  ASSERT_EQ(data.LoadData(::kDataPath), ErrorCode::kSuccess);
  ASSERT_EQ(data.GetAssetDataByNames(kAssetNameQuery, asset_data),
            ErrorCode::kSuccess);
  ASSERT_EQ(asset_data.size(), 4);
  for (const auto &asset : asset_data) {
    ASSERT_THAT(kAssetNames, ::testing::Contains(asset->GetName()));
    ASSERT_THAT(kAssetData[asset->GetName()],
                ::testing::ContainerEq(asset->GetChangeRates()));
  }
}

TEST(DataTransformerTest, PrepareEmptyData) {
  DataTransformer data_transformer;
  std::vector<const IAsset *> data;
  std::vector<std::unique_ptr<IAsset>> prepared_data;
  ASSERT_EQ(data_transformer.PrepareData(data, prepared_data),
            asset_optimization_tool::ErrorCode::kInvalidData);
}

TEST(DataTransformerTest, PrepareData) {
  DataTransformer data_transformer;
  Data data;
  std::vector<const IAsset *> asset_data;
  std::vector<std::unique_ptr<IAsset>> prepared_data;
  data.LoadData(::kDataPath);
  data.GetAssetDataByNames(kAssetNames, asset_data);
  ASSERT_EQ(data_transformer.PrepareData(asset_data, prepared_data),
            asset_optimization_tool::ErrorCode::kSuccess);
  for (size_t i = 0; i < asset_data.size(); ++i) {
    ASSERT_EQ(asset_data[i]->GetName(), prepared_data[i]->GetName());
    ASSERT_THAT(asset_data[i]->GetChangeRates(),
                ::testing::ContainerEq(prepared_data[i]->GetChangeRates()));
  }
}