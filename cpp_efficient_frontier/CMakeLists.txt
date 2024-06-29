cmake_minimum_required(VERSION 3.10)

project(efficient_frontier)

add_subdirectory("calculations")
add_subdirectory("data_manipulation")
add_subdirectory(3rdparty/RapidFuzz)

add_executable(main_program "program.cpp" "program.hpp")

target_link_libraries(main_program lib_calculations)
target_link_libraries(main_program lib_data)
