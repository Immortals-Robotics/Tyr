project(${CMAKE_PROJECT_NAME}-strategy)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

set(HEADER_FILES
        pch.h
        application.h
        frame.h
        strategy.h
        robot.h
        gui.h
        waypoint.h
        sync_data.h
)

set(SOURCE_FILES
        main.cpp
        frame.cpp
        strategy.cpp
        robot.cpp
        gui.cpp
)

add_executable(${PROJECT_NAME}
        ${HEADER_FILES}
        ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
        ${CMAKE_PROJECT_NAME}-common
        Qt5::Core Qt5::Widgets
)

set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${USE_UNITY_BUILDS})

get_target_property(COMMON_PCHS ${CMAKE_PROJECT_NAME}-common INTERFACE_PRECOMPILE_HEADERS)

target_precompile_headers(${PROJECT_NAME}
        PRIVATE ${COMMON_PCHS}
        PRIVATE pch.h)

install(TARGETS ${PROJECT_NAME})
