enable_testing()

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(TEST_SOURCES
    testcases/test_analyzer.cpp
    testcases/test_data.cpp
    testcases/test_tool_interface.cpp
    testcases/test_tool_logic.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/module/
)

add_executable(run_tests ${TEST_SOURCES})

target_link_libraries(run_tests gtest gmock gtest_main analyzer data asset_optimization_tool)
configure_file(${CMAKE_SOURCE_DIR}/ut/testfiles/data_example.tsv ${CMAKE_CURRENT_BINARY_DIR}/data_example.tsv COPYONLY)

add_test(NAME run_tests COMMAND run_tests)