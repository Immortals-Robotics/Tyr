project(${CMAKE_PROJECT_NAME}-referee)

find_package(immortals-common CONFIG REQUIRED)

set(HEADER_FILES
        pch.h
        referee.h)

set(SOURCE_FILES
        referee.cpp)

add_library(${PROJECT_NAME}
        ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC
        immortals::common)

set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${USE_UNITY_BUILDS})

target_precompile_headers(${PROJECT_NAME}
        PUBLIC pch.h
        INTERFACE ${HEADER_FILES})
