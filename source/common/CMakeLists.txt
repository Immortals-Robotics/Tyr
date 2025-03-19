project(${CMAKE_PROJECT_NAME}-common CXX)

set(libs)

find_package(asio CONFIG REQUIRED)
list(APPEND libs asio::asio)
find_package(nng CONFIG REQUIRED)
list(APPEND libs nng::nng)
find_package(unofficial-lmdb CONFIG REQUIRED)
list(APPEND libs unofficial::lmdb::lmdb)
find_package(raylib CONFIG REQUIRED)
list(APPEND libs raylib)
find_package(imgui CONFIG REQUIRED)
list(APPEND libs imgui::imgui)
find_package(spdlog CONFIG REQUIRED)
list(APPEND libs spdlog::spdlog)
find_package(PkgConfig REQUIRED)
pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus)
list(APPEND libs PkgConfig::tomlplusplus)
find_package(xxHash CONFIG REQUIRED)
list(APPEND libs xxHash::xxhash)

find_package(immortals-protos CONFIG REQUIRED)
list(APPEND libs immortals::protos)

add_library(${PROJECT_NAME} "")

set(HEADER_FILES
        pch.h
        services.h

        debugging/thread_name.h

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
        math/geom/robot.h
        math/geom/triangle.h

        network/address.h

        state/field/ball_model.h
        state/field/camera_calibration.h
        state/field/field.h
        state/raw/ball.h
        state/raw/frame.h
        state/raw/robot.h
        state/raw/world.h
        state/referee/command.h
        state/referee/match.h
        state/referee/state.h
        state/referee/team_info.h
        state/soccer/robot.h
        state/soccer/state.h
        state/world/ball.h
        state/world/robot.h
        state/world/seen_state.h
        state/world/world.h

        time/duration.h
        time/time_point.h
        time/timer.h

        config/base.h
        config/common.h
        config/network.h
        config/config.h
        config/soccer.h
        config/vision.h

        utility/enum_flags.h
)

set(SOURCE_FILES
        services.cpp

        math/linear.cpp
        math/geom/circle.cpp
        math/geom/line.cpp)

if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC _WIN32_WINNT=_WIN32_WINNT_WIN10)
endif ()
list(APPEND HEADER_FILES
        network/udp_client.h
        network/udp_server.h)
list(APPEND SOURCE_FILES
        network/udp_client.cpp
        network/udp_server.cpp)

list(APPEND HEADER_FILES
        network/nng_client.h
        network/nng_server.h
        network/nng_message.h)
list(APPEND SOURCE_FILES
        network/nng_client.cpp
        network/nng_server.cpp)

list(APPEND HEADER_FILES
        storage/storage.h)
list(APPEND SOURCE_FILES
        storage/storage.cpp)


    list(APPEND HEADER_FILES
            storage/dumper.h)
    list(APPEND SOURCE_FILES
            storage/dumper.cpp)

list(APPEND HEADER_FILES
        logging/logging.h)
list(APPEND SOURCE_FILES
        logging/logging.cpp)

    list(APPEND HEADER_FILES
            logging/debug_sink.h)

list(APPEND HEADER_FILES
        config/file.h)
list(APPEND SOURCE_FILES
        config/file.cpp)

list(APPEND HEADER_FILES
        debugging/color.h
        debugging/draw.h
        debugging/execution_time.h
        debugging/hub.h
        debugging/log.h
        debugging/source_location.h
        debugging/wrapper.h)

target_sources(${PROJECT_NAME} PRIVATE ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC ${libs})

set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${USE_UNITY_BUILDS})

target_precompile_headers(${PROJECT_NAME}
        PUBLIC pch.h
        INTERFACE ${HEADER_FILES})
