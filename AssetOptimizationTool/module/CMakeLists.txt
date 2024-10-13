include_directories(${CMAKE_SOURCE_DIR}/module)

add_subdirectory("analyzer")
add_subdirectory("asset_mixer")
add_subdirectory("configuration")
add_subdirectory("data")
add_subdirectory("simulation_data_manager")
add_subdirectory("simulator")

add_library(asset_optimization_tool SHARED asset_optimization_tool_impl.cpp)
target_link_libraries(asset_optimization_tool analyzer asset_mixer configuration data simulation_data_manager simulator)

