add_subdirectory(expected_returns)

add_library(calculations calculations.h calculations.cpp)
target_link_libraries(calculations expected_returns)
