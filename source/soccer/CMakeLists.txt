project(${CMAKE_PROJECT_NAME}-soccer)

set(HEADER_FILES
    pch.h
    ai.h
    errt/errt.h
    errt/obstacle_new.h
    errt/obstacle.h
    errt/tree.h
    robot/robot.h
    robot/trapezoid.h
    robot/velocity_profile.h)

set(SOURCE_FILES
    ai.cpp
    ai_process.cpp
    errt/errt.cpp
    errt/obstacle_circle.cpp
    errt/obstacle_map.cpp
    errt/obstacle_rect.cpp
    errt/obstacle.cpp
    errt/tree.cpp
    robot/robot.cpp
    robot/trapezoid.cpp)

add_library(${PROJECT_NAME}
    ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC
	${CMAKE_PROJECT_NAME}-common
	${CMAKE_PROJECT_NAME}-sender)

if (${TRACE_BUILD_TIME})
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "[Cc]lang")
    target_compile_options(${PROJECT_NAME} PUBLIC -ftime-trace)
endif()
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${USE_UNITY_BUILDS})

target_precompile_headers(${PROJECT_NAME} PUBLIC pch.h)
