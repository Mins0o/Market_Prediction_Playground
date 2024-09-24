#include "data/data.h"  // testing class
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "types.h"

using ::asset_optimization_tool::ErrorCode;
using ::asset_optimization_tool::modules::Data;
using ::testing::Return;

// Example test case for the Analyzer class
TEST(DataTest, LoadMethod) {
  Data data;

  ASSERT_EQ(data.LoadData("./build/ut/data_example.tsv"),
            ErrorCode::kSuccess);  // Test if it doesn't throw exceptions
}
