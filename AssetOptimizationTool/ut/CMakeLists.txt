enable_testing()

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(TEST_SOURCES
    testcases/test_analyzer.cpp
    testcases/test_data.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/module/analyzer
    ${CMAKE_SOURCE_DIR}/module/asset_mixer
    ${CMAKE_SOURCE_DIR}/module/configurator
    ${CMAKE_SOURCE_DIR}/module/data
    ${CMAKE_SOURCE_DIR}/module/simulation_data_manager
    ${CMAKE_SOURCE_DIR}/module/simulator
    ${CMAKE_SOURCE_DIR}/module/_interfaces
)

add_executable(run_tests ${TEST_SOURCES})

target_link_libraries(run_tests gtest gmock gtest_main analyzer data)

add_test(NAME run_tests COMMAND run_tests)