project(${CMAKE_PROJECT_NAME}-gui)

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
	menu/widget_menu.cpp
	menu/demo_menu.cpp
	renderer/renderer_ball.cpp
	renderer/renderer_robot.cpp
	renderer/renderer.cpp
	renderer/renderer_debug.cpp
)

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE
	${CMAKE_PROJECT_NAME}-common
	${CMAKE_PROJECT_NAME}-vision
	${CMAKE_PROJECT_NAME}-referee
	${CMAKE_PROJECT_NAME}-sender
	${CMAKE_PROJECT_NAME}-soccer
	raylib
	imgui::imgui
	rlimgui::rlimgui)

set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${USE_UNITY_BUILDS})

get_target_property(COMMON_PCHS ${CMAKE_PROJECT_NAME}-common INTERFACE_PRECOMPILE_HEADERS)

target_precompile_headers(${PROJECT_NAME}
	PRIVATE ${COMMON_PCHS}
	PRIVATE pch.h)

install(TARGETS ${PROJECT_NAME})
