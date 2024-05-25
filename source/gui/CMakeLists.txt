project(${CMAKE_PROJECT_NAME}-gui)

find_package(rlimgui CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)

set(HEADER_FILES
        pch.h
        app.h
        renderer/renderer.h
        renderer/draw_inl.h
        menu/config_menu.h
        menu/widget_menu.h
        menu/demo_menu.h
        menu/footer_menu.h
)

set(SOURCE_FILES
        main.cpp
        app.cpp
        menu/config_menu.cpp
        menu/widget_menu.cpp
        menu/demo_menu.cpp
        menu/footer_menu.cpp
        renderer/ball.cpp
        renderer/debug.cpp
        renderer/field.cpp
        renderer/renderer.cpp
        menu/filter_tab.cpp
        renderer/robot.cpp
        renderer/info.cpp
        menu/plot.cpp
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
