cmake_minimum_required(VERSION 3.10)

project(AssetOptimizationTool)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

# Optimization flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Static analysis (optional)
# set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=clang-analyzer-*, \
#                                     -clang-analyzer-deadcode.*, \
#                                     -clang-analyzer-core.*, \
#                                     -llvm-*,-readability-*, \
#                                     -modernize-*, \
#                                     -performance-*, \
#                                     -misc-*, \
#                                     -bugprone-*, \
#                                     -cppcoreguidelines-*, \
#                                     -fuchsia-*, \
#                                     -clang-diagnostic-*")

# Position Independent Code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(${CMAKE_SOURCE_DIR}/interface)

add_subdirectory("module")
add_subdirectory("ut")

include_directories("interface")