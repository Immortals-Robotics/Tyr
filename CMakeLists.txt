cmake_minimum_required(VERSION 3.22)

set(USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

project(tyr)

option(USE_UNITY_BUILDS "Enable unity build to improve build times" OFF)
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	option(TRACE_BUILD_TIME "Use -ftime-trace to generate build time trace" OFF)
else()
	set(TRACE_BUILD_TIME OFF)
endif()
option(ENABLE_SANITIZERS "Enable address and undefined behavior sanitizers" OFF)


# Raylib's vcpkg port forces sanitizers to be on in debug,
# we therefore need to force enable it in debug to avoid linking issues
if(${ENABLE_SANITIZERS} OR CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_options(-fsanitize=address)
	add_link_options(-fsanitize=address)

	# undefined behavior sanitizer is not available on MSVC
	if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		add_compile_options(-fno-omit-frame-pointer -fsanitize=undefined)
		add_link_options(-fno-omit-frame-pointer -fsanitize=undefined)
	endif()
endif()

if (${TRACE_BUILD_TIME})
	add_compile_options(-ftime-trace)
endif()

add_subdirectory(source/protos)
add_subdirectory(source/common)
add_subdirectory(source/vision)
add_subdirectory(source/referee)
add_subdirectory(source/sender)
add_subdirectory(source/soccer)
add_subdirectory(source/gui)

set(CPACK_GENERATOR ZIP)
set(CPACK_OUTPUT_FILE_PREFIX ${CMAKE_BINARY_DIR}/dist)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
include(CPack)
