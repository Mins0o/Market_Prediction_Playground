add_library(analysis SHARED "analysis.cpp" "analysis.h")
target_link_libraries(analysis PRIVATE data)