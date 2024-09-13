#include "data.h"  // testing class
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::asset_optimization_tool::modules::Data;
using ::testing::Return;

// Example test case for the Analyzer class
TEST(DataTest, LoadMethod) {
  Data data;

  EXPECT_NO_THROW(data.LoadData());  // Test if it doesn't throw exceptions
}
