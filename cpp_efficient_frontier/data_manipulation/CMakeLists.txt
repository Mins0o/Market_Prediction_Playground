add_library(lib_data "data_manipulation.cpp" "data_manipulation.h")
target_link_libraries(lib_data PRIVATE rapidfuzz::rapidfuzz)