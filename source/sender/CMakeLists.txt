project(${CMAKE_PROJECT_NAME}-sender)

find_package(Protobuf REQUIRED)
find_package(ZeroMQ CONFIG QUIET)

protobuf_generate_cpp(ZMQ_PROTO_SRCS ZMQ_PROTO_HDRS
        ${CMAKE_SOURCE_DIR}/../MainBoardFirmware/shared/zmq_protocol.proto)

set(ZMQ_LIB_TARGET zmq)
if (TARGET libzmq)
    set(ZMQ_LIB_TARGET libzmq)
elseif (TARGET libzmq-static)
    set(ZMQ_LIB_TARGET libzmq-static)
endif ()

set(HEADER_FILES
        pch.h
        base.h
        command.h
        grsim.h
        simulator.h
        zmq_robot_sender.h
        hub.h
        nrf.h
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
        grsim.cpp
        nrf.cpp
        simulator.cpp
        zmq_robot_sender.cpp
        ${ZMQ_PROTO_SRCS}
        ${C_SOURCE_FILES})

add_library(${PROJECT_NAME}
        ${SOURCE_FILES} ${HEADER_FILES} ${ZMQ_PROTO_HDRS})

target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${CMAKE_SOURCE_DIR}/../MainBoardFirmware/shared")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
        grsim.cpp
        nrf.cpp
        simulator.cpp
        zmq_robot_sender.cpp
        ${C_SOURCE_FILES}
        ${HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_BINARY_DIR} PREFIX generated FILES
        ${ZMQ_PROTO_SRCS}
        ${ZMQ_PROTO_HDRS})

target_link_libraries(${PROJECT_NAME} PUBLIC
        ${CMAKE_PROJECT_NAME}-common
        protobuf::libprotobuf
        ${ZMQ_LIB_TARGET})

set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${USE_UNITY_BUILDS})

set_source_files_properties(${C_SOURCE_FILES} PROPERTIES
        SKIP_PRECOMPILE_HEADERS TRUE
        SKIP_UNITY_BUILD_INCLUSION TRUE)

get_target_property(COMMON_PCHS ${CMAKE_PROJECT_NAME}-common INTERFACE_PRECOMPILE_HEADERS)

target_precompile_headers(${PROJECT_NAME}
        PUBLIC ${COMMON_PCHS}
        PUBLIC pch.h
        INTERFACE ${HEADER_FILES})
