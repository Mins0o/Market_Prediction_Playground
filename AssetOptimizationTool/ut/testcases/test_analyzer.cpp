#include "analyzer/analyzer.h"  // testing class
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::asset_optimization_tool::modules::Analyzer;

// Example test case for the Analyzer class
TEST(AnalyzerTest, AnalyzeMethod) {
  Analyzer analyzer;

  EXPECT_NO_THROW(analyzer.analyze());  // Test if it doesn't throw exceptions
}
