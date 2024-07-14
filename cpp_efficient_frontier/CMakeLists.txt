cmake_minimum_required(VERSION 3.10)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project(efficient_frontier)

add_subdirectory("calculations")
add_subdirectory("data_manipulation")
add_subdirectory("analysis")
add_subdirectory(3rdparty/RapidFuzz)

add_executable(main_program "program.cpp")

target_link_libraries(main_program calculations)
target_link_libraries(main_program data)
target_link_libraries(main_program analysis)
