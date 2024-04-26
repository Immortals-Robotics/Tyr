project(${CMAKE_PROJECT_NAME}-sender)

set(HEADER_FILES
    pch.h
    sender.h
    protocol/data_lite.h
    protocol/defines.h
    protocol/half.h
    protocol/reader.h
    protocol/writer.h)

set(C_SOURCE_FILES
    protocol/half.c
    protocol/reader.c
    protocol/writer.c)

set(SOURCE_FILES
    sender.cpp
    ${C_SOURCE_FILES})

add_library(${PROJECT_NAME}
    ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC
	${CMAKE_PROJECT_NAME}-common)

if (${TRACE_BUILD_TIME})
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "[Cc]lang")
    target_compile_options(${PROJECT_NAME} PUBLIC -ftime-trace)
endif()
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${USE_UNITY_BUILDS})

set_source_files_properties(${C_SOURCE_FILES} PROPERTIES 
    SKIP_PRECOMPILE_HEADERS TRUE
    SKIP_UNITY_BUILD_INCLUSION TRUE)

target_precompile_headers(${PROJECT_NAME} PUBLIC pch.h)
