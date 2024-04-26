project(${CMAKE_PROJECT_NAME}-gui)

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
endif()

find_package(raylib CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(rlimgui CONFIG REQUIRED)

file(GLOB_RECURSE GEN_SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.c" "*.cpp")
file(GLOB_RECURSE GEN_HEADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h" "*.hpp")

set(SOURCE_FILES
    ${GEN_HEADER_FILES}
    ${GEN_SOURCE_FILES})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE
	${CMAKE_PROJECT_NAME}-common
	raylib
	imgui::imgui
	rlimgui::rlimgui)

if (${TRACE_BUILD_TIME})
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "[Cc]lang")
	target_compile_options(${PROJECT_NAME} PUBLIC -ftime-trace)
endif()
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${USE_UNITY_BUILDS})

target_precompile_headers(${PROJECT_NAME} PRIVATE "pch.h")

install(TARGETS ${PROJECT_NAME})
