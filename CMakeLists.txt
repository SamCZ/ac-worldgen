cmake_minimum_required(VERSION 3.20)

project(ac_worldgen_master VERSION 0.0.3 LANGUAGES CXX)

option(AC_USE_TRACY "Use tracy" OFF)
option(AC_BUILD_APP "Use tracy" ON)

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
	add_compile_options(-W) #,-mbig-obj
endif()

file(GLOB_RECURSE PROJECT_SOURCE_FILES src/*.h src/*.hpp src/*.cpp src/*.cc)
add_library(ac_worldgen STATIC ${PROJECT_SOURCE_FILES})
target_include_directories(ac_worldgen PUBLIC src)

target_precompile_headers(ac_worldgen PUBLIC "src/pch.h")

target_compile_definitions(ac_worldgen PUBLIC _USE_MATH_DEFINES)
target_compile_definitions(ac_worldgen PUBLIC _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)

# If enabled outputs file after preprocessing
#target_compile_options(ac_worldgen PRIVATE -E)

target_link_libraries(ac_worldgen PUBLIC antlr4_static FastNoise fmt)

if (AC_USE_TRACY)
	target_compile_definitions(ac_worldgen PUBLIC AC_ENABLE_TRACY=1)
	target_link_libraries(ac_worldgen PUBLIC TracyClient)
endif ()

if (AC_BUILD_APP)
	add_subdirectory(app)
endif ()