project(ac_worldgen_app VERSION 0.0.3 LANGUAGES CXX)

add_executable(ac_worldgen_app main.cpp)
target_link_libraries(ac_worldgen_app ac_worldgen)