project(${CMAKE_PROJECT_NAME}-common)

find_package(asio CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(unofficial-lmdb CONFIG REQUIRED)
find_package(nng CONFIG REQUIRED)
find_package(xxHash CONFIG REQUIRED)

# needed to define type conversions
find_package(raylib CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus)

set(HEADER_FILES
        pch.h
        services.h

        config/base.h
        config/common.h
        config/file.h
        config/network.h
        config/config.h
        config/soccer.h
        config/vision.h

        debugging/color.h
        debugging/draw.h
        debugging/hub.h
        debugging/log.h
        debugging/source_location.h
        debugging/wrapper.h

        logging/debug_sink.h
        logging/logging.h
        logging/macros.h

        math/angle.h
        math/helpers.h
        math/linear.h
        math/median_filter.h
        math/random.h
        math/vec2.h
        math/vec3.h
        math/geom/circle.h
        math/geom/line_segment.h
        math/geom/line.h
        math/geom/rect.h
        math/geom/triangle.h

        network/address.h
        network/nng_client.h
        network/nng_server.h
        network/udp_client.h
        network/nng_message.h
        network/udp_server.h

        state/referee.h
        state/world.h
        storage/dumper.h
        storage/storage.h

        time/duration.h
        time/time_point.h
        time/timer.h)

set(SOURCE_FILES
        services.cpp

        config/file.cpp

        logging/logging.cpp
        math/linear.cpp
        math/geom/circle.cpp
        math/geom/line.cpp
        network/nng_client.cpp
        network/nng_server.cpp
        network/udp_client.cpp
        network/udp_server.cpp
        storage/dumper.cpp
        storage/storage.cpp)

add_library(${PROJECT_NAME}
        ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC
        ${CMAKE_PROJECT_NAME}-protos
        asio::asio
        PkgConfig::tomlplusplus
        spdlog::spdlog
        unofficial::lmdb::lmdb
        nng::nng
        xxHash::xxhash
        raylib
        imgui::imgui)

if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC _WIN32_WINNT=_WIN32_WINNT_WIN10)
endif ()

target_compile_definitions(${PROJECT_NAME} PUBLIC DATA_DIR="${CMAKE_SOURCE_DIR}/data")
target_compile_definitions(${PROJECT_NAME} PUBLIC LOG_DIR="${CMAKE_SOURCE_DIR}/log")

set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${USE_UNITY_BUILDS})

get_target_property(PROTOS_PCHS ${CMAKE_PROJECT_NAME}-protos INTERFACE_PRECOMPILE_HEADERS)

target_precompile_headers(${PROJECT_NAME}
        PUBLIC ${PROTOS_PCHS}
        PUBLIC pch.h
        INTERFACE ${HEADER_FILES})
