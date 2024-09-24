enable_testing()

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(TEST_SOURCES
    testcases/test_analyzer.cpp
    testcases/test_data.cpp
    testcases/test_tool_integrated.cpp
    testcases/test_tool_logic.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/module/
)

add_executable(run_tests ${TEST_SOURCES})

target_link_libraries(run_tests gtest gmock gtest_main analyzer data asset_optimization_tool)
file(GLOB TEST_FILES "${CMAKE_SOURCE_DIR}/ut/testfiles/*")
file(COPY ${TEST_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

add_test(NAME run_tests COMMAND run_tests)