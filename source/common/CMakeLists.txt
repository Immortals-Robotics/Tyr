project(${CMAKE_PROJECT_NAME}-common)

find_package(asio CONFIG REQUIRED)
find_package(quill CONFIG REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus)

set(HEADER_FILES
    pch.h
    services.h
    setting.h
    config/config.h
    debug/common_colors.h
    debug/debug.h
    logging/logging.h
    math/angle.h
    math/helpers.h
    math/linear.h
    math/median_filter.h
    math/random.h
    math/vector.h
    math/geom/circle.h
    math/geom/line.h
    math/geom/rect.h
    network/udp_client.h
    network/udp_server.h
    state/game_commands.h
    state/game.h
    state/referee.h
    state/world.h
    timer/timer.h)

set(SOURCE_FILES
    config/config.cpp
    debug/common_colors.cpp
    debug/debug.cpp
    logging/logging.cpp
    math/angle.cpp
    math/helpers.cpp
    math/linear.cpp
    math/random.cpp
    math/vector.cpp
    math/geom/circle.cpp
    math/geom/line.cpp
    math/geom/rect.cpp
    network/udp_client.cpp
    network/udp_server.cpp
    timer/timer.cpp)

add_library(${PROJECT_NAME}
    ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC
    ${CMAKE_PROJECT_NAME}-protos
    asio::asio
    PkgConfig::tomlplusplus
    quill::quill)

target_compile_definitions(${PROJECT_NAME} PUBLIC TOML_EXCEPTIONS=0)
if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC _WIN32_WINNT=_WIN32_WINNT_WIN10)
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC QUILL_ROOT_LOGGER_ONLY)

if (${TRACE_BUILD_TIME})
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "[Cc]lang")
    target_compile_options(${PROJECT_NAME} PUBLIC -ftime-trace)
endif()
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC DATA_DIR="${CMAKE_SOURCE_DIR}/data")
target_compile_definitions(${PROJECT_NAME} PUBLIC LOG_DIR="${CMAKE_SOURCE_DIR}/log")

set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${USE_UNITY_BUILDS})

target_precompile_headers(${PROJECT_NAME} PUBLIC ${HEADER_FILES})
