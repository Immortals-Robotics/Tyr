project(${CMAKE_PROJECT_NAME}-gui)

# TODO: find out why this is needed and fix it
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
endif()

find_package(raylib CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(rlimgui CONFIG REQUIRED)

set(HEADER_FILES
	pch.h
	app.h
	renderer/renderer.h
	menu/config_menu.h
)

set(SOURCE_FILES
	main.cpp
	app.cpp
	menu/config_menu.cpp
	renderer/renderer_ball.cpp
	renderer/renderer_robot.cpp
	renderer/renderer.cpp
)

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

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

file(COPY ${CMAKE_SOURCE_DIR}/source/gui/shaders DESTINATION ${CMAKE_BINARY_DIR}/source/gui)
file(COPY ${CMAKE_SOURCE_DIR}/source/gui/fonts DESTINATION ${CMAKE_BINARY_DIR}/source/gui)
