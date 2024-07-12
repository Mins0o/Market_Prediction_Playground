add_library(lib_analysis SHARED "analysis.cpp" "analysis.h")
target_link_libraries(lib_analysis PRIVATE lib_data)