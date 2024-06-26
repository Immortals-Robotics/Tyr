project(${CMAKE_PROJECT_NAME}-cli)

set(HEADER_FILES
        pch.h
        app.h
)

set(SOURCE_FILES
        main.cpp
        app.cpp
)

add_executable(${PROJECT_NAME}
        ${HEADER_FILES}
        ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${CMAKE_PROJECT_NAME}-vision
        ${CMAKE_PROJECT_NAME}-referee
        ${CMAKE_PROJECT_NAME}-sender
        ${CMAKE_PROJECT_NAME}-soccer)

set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${USE_UNITY_BUILDS})

get_target_property(VISION_PCHS ${CMAKE_PROJECT_NAME}-vision INTERFACE_PRECOMPILE_HEADERS)
get_target_property(REFEREE_PCHS ${CMAKE_PROJECT_NAME}-referee INTERFACE_PRECOMPILE_HEADERS)
get_target_property(SENDER_PCHS ${CMAKE_PROJECT_NAME}-sender INTERFACE_PRECOMPILE_HEADERS)
get_target_property(SOCCER_PCHS ${CMAKE_PROJECT_NAME}-soccer INTERFACE_PRECOMPILE_HEADERS)

target_precompile_headers(${PROJECT_NAME}
        PUBLIC ${VISION_PCHS}
        PUBLIC ${REFEREE_PCHS}
        PUBLIC ${SENDER_PCHS}
        PUBLIC ${SOCCER_PCHS}
        PRIVATE pch.h)

install(TARGETS ${PROJECT_NAME})
