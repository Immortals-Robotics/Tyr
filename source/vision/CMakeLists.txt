project(${CMAKE_PROJECT_NAME}-vision)

set(HEADER_FILES
    pch.h
    vision.h
    kalman/filtered_object.h)

set(SOURCE_FILES
    vision_ball.cpp
    vision_comminucation.cpp
    vision_param.cpp
    vision_robot.cpp
    vision.cpp)

add_library(${PROJECT_NAME}
    ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC
	${CMAKE_PROJECT_NAME}-common)

option(TRACE_BUILD_TIME "Use -ftime-trace to generate build time trace" OFF)
if (${TRACE_BUILD_TIME})
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "[Cc]lang")
    target_compile_options(${PROJECT_NAME} PUBLIC -ftime-trace)
endif()
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${USE_UNITY_BUILDS})

target_precompile_headers(${PROJECT_NAME} PUBLIC pch.h)
