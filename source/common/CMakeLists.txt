project(${CMAKE_PROJECT_NAME}-common)

find_package(asio CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus)

set(HEADER_FILES
	pch.h
	services.h
	setting.h
	config/config.h
	debugging/common_colors.h
	debugging/debug.h
	logging/logging.h
	logging/macros.h
	math/angle.h
	math/helpers.h
	math/linear.h
	math/median_filter.h
	math/random.h
	math/vector.h
	math/geom/circle.h
	math/geom/line_segment.h
	math/geom/line.h
	math/geom/rect.h
	network/udp_client.h
	network/udp_server.h
	state/game.h
	state/referee.h
	state/world.h
	timer/timer.h)

set(SOURCE_FILES
	setting.cpp
	config/config.cpp
	debugging/common_colors.cpp
	debugging/debug.cpp
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
	spdlog::spdlog)

if(WIN32)
	target_compile_definitions(${PROJECT_NAME} PUBLIC _WIN32_WINNT=_WIN32_WINNT_WIN10)
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC DATA_DIR="${CMAKE_SOURCE_DIR}/data")
target_compile_definitions(${PROJECT_NAME} PUBLIC LOG_DIR="${CMAKE_SOURCE_DIR}/log")

set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${USE_UNITY_BUILDS})

get_target_property(PROTOS_PCHS ${CMAKE_PROJECT_NAME}-protos INTERFACE_PRECOMPILE_HEADERS)

target_precompile_headers(${PROJECT_NAME}
	PUBLIC ${PROTOS_PCHS}
	PUBLIC pch.h
	INTERFACE ${HEADER_FILES})
