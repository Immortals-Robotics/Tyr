project(${CMAKE_PROJECT_NAME}-vision)

set(HEADER_FILES
	pch.h
	vision.h
	kalman/filtered_object.h)

set(SOURCE_FILES
	vision_ball.cpp
	vision_communication.cpp
	vision_robot.cpp
	vision.cpp
	kalman/filtered_object.cpp)

add_library(${PROJECT_NAME}
	${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC
	${CMAKE_PROJECT_NAME}-common)

set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${USE_UNITY_BUILDS})

get_target_property(COMMON_PCHS ${CMAKE_PROJECT_NAME}-common INTERFACE_PRECOMPILE_HEADERS)

target_precompile_headers(${PROJECT_NAME}
	PUBLIC ${COMMON_PCHS}
	PUBLIC pch.h
	INTERFACE ${HEADER_FILES})
