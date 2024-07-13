add_subdirectory(date_line)

add_library(data "data_manipulation.cpp" "data_manipulation.h")
target_link_libraries(data PRIVATE rapidfuzz::rapidfuzz)
target_link_libraries(data PRIVATE date_line)