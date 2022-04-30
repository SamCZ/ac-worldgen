if (NOT TARGET antlr)
	project(antlr C CXX)
	set(ANTLR_PROJECT_DIR antlr/runtime/Cpp/runtime/src)

	file(GLOB_RECURSE ANTLR_SOURCES ${ANTLR_PROJECT_DIR}/*.cpp)

	add_library(antlr4_static STATIC ${ANTLR_SOURCES})
	target_include_directories(antlr4_static PUBLIC ${ANTLR_PROJECT_DIR})

	target_compile_definitions(antlr4_static PUBLIC ANTLR4CPP_STATIC)

	if(CMAKE_SYSTEM_NAME MATCHES "Linux")
		target_link_libraries(antlr4_static ${UUID_LIBRARIES})
	elseif(APPLE)
		target_link_libraries(antlr4_static ${COREFOUNDATION_LIBRARY})
	endif()

	if (MSVC)
		add_compile_options(/wd4251)
	else()
		add_compile_options(-Wno-overloaded-virtual)
	endif()
endif()

if(NOT TARGET FastNoise)
	set(FASTNOISE2_NOISETOOL OFF)
	set(FASTNOISE2_TESTS OFF)
	add_subdirectory(fastNoise2)
endif()

if(NOT TARGET TracyClient)
	if(AC_ENABLE_TRACY)
		set(TRACY_ENABLE ON)
	else()
		set(TRACY_ENABLE OFF)
	endif()

	set(BUILD_SHARED_LIBS_CACHE ${BUILD_SHARED_LIBS})
	set(BUILD_SHARED_LIBS OFF)

	add_subdirectory(tracy)
	set_property(TARGET TracyClient PROPERTY FOLDER "GameLib")
	if(WIN32)
		target_link_libraries(TracyClient PUBLIC Ws2_32 dbghelp)
		target_compile_definitions(TracyClient PUBLIC NOMINMAX)
	endif()
	set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_CACHE})
endif()