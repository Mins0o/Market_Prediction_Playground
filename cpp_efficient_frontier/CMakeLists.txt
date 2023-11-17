cmake_minimum_required(VERSION 3.10)

project(efficient_frontier)

add_subdirectory("data_reading_sandbox")
add_subdirectory("calculations")
add_executable(main_program "program.cpp")

target_link_libraries(main_program lib_data_reading_sandbox)
target_link_libraries(main_program lib_calculations)