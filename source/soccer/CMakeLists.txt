project(${CMAKE_PROJECT_NAME}-soccer)

set(HEADER_FILES
    pch.h
    ai.h
    one_touch_detector.h
    dss/dss.h
    dss/parabolic.h
    dss/trajectory.h
    errt/errt.h
    errt/tree.h
    obstacle/obstacle_new.h
    obstacle/obstacle.h
    robot/robot.h
    robot/trapezoid.h
    robot/velocity_profile.h)

set(SOURCE_FILES
    ai.cpp
    ai_process.cpp
    dss/dss.cpp
    dss/parabolic.cpp
    dss/trajectory.cpp
    errt/errt.cpp
    errt/tree.cpp
    obstacle/obstacle_circle.cpp
    obstacle/obstacle_map.cpp
    obstacle/obstacle_rect.cpp
    obstacle/obstacle.cpp
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
