cmake_minimum_required(VERSION 3.20)

project(ac_worldgen_master VERSION 0.0.3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

if (MSVC)
	set(CMAKE_CXX20_STANDARD_COMPILE_OPTION "-std:c++latest")
	set(CMAKE_CXX20_EXTENSION_COMPILE_OPTION "-std:c++latest")
endif()

add_subdirectory(deps)

if (MSVC)
	add_compile_options(/bigobj)
else()
	add_compile_options(-Wa,-mbig-obj)
endif()

file(GLOB_RECURSE PROJECT_SOURCE_FILES src/*.h src/*.hpp src/*.cpp src/*.cc)
add_library(ac_worldgen STATIC ${PROJECT_SOURCE_FILES})
target_include_directories(ac_worldgen PUBLIC src)

target_precompile_headers(ac_worldgen PUBLIC "src/pch.h")

target_compile_definitions(ac_worldgen PUBLIC _USE_MATH_DEFINES)
target_compile_definitions(ac_worldgen PUBLIC _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)

target_link_libraries(ac_worldgen PUBLIC antlr4_static FastNoise TracyClient)

add_subdirectory(app)