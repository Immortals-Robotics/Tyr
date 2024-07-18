project(${CMAKE_PROJECT_NAME}-gui)

find_package(rlimgui CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)

set(HEADER_FILES
        pch.h
        app.h

        menu/config.h
        menu/controller.h
        menu/demo.h
        menu/filter.h
        menu/log.h
        menu/plot.h
        menu/soccer.h
        menu/status_bar.h

        renderer/renderer.h
        renderer/draw_inl.h

        theme/imgui_theme.h
)

set(SOURCE_FILES
        main.cpp
        app.cpp

        menu/config.cpp
        menu/controller.cpp
        menu/demo.cpp
        menu/filter.cpp
        menu/log.cpp
        menu/plot.cpp
        menu/soccer.cpp
        menu/status_bar.cpp

        renderer/ball.cpp
        renderer/debug.cpp
        renderer/field.cpp
        renderer/info.cpp
        renderer/renderer.cpp
        renderer/robot.cpp

        theme/imgui_theme.cpp
)

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${CMAKE_PROJECT_NAME}-vision
        ${CMAKE_PROJECT_NAME}-referee
        ${CMAKE_PROJECT_NAME}-sender
        ${CMAKE_PROJECT_NAME}-soccer
        rlimgui::rlimgui
        implot::implot)

set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${USE_UNITY_BUILDS})

target_precompile_headers(${PROJECT_NAME}
        PRIVATE pch.h)

install(TARGETS ${PROJECT_NAME})
