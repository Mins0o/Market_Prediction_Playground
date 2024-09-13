cmake_minimum_required(VERSION 3.10)

project(AssetOptimizationTool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR}/interface)

add_subdirectory("module")
add_subdirectory("ut")

include_directories("interface")