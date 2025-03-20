project(${CMAKE_PROJECT_NAME}-soccer)

find_package(LEMON CONFIG REQUIRED)

set(HEADER_FILES
        pch.h
        ai.h
        state.h

        navigation/obstacle/circle.h
        navigation/obstacle/map.h
        navigation/obstacle/rect.h

        navigation/planner/planner.h

        navigation/trajectory/trajectory.h
        navigation/trajectory/trajectory_chained.h
        navigation/trajectory/trajectory_constant_acc.h
        navigation/trajectory/trajectory_pieced.h
        navigation/trajectory/trajectory_2d.h

        plays/play_book.h

        helpers/one_touch_detector.h
        helpers/open_angle.h

        robot/robot.h
        robot/velocity_profile.h

        skills/skill.h
        skills/halt.h
        skills/mark_to_ball.h
        skills/mark_to_goal.h
        skills/wait_for_pass.h

        tactics/tactic.h
        tactics/mark.h
)

set(SOURCE_FILES
        ai.cpp
        ai_process.cpp
        internal_process_data.cpp

        assignment/assignment.cpp

        assignment/cost_attack.cpp
        assignment/cost_gk.cpp
        assignment/cost_mark.cpp
        assignment/cost_static.cpp

        assignment/create_attack.cpp
        assignment/create_defs.cpp
        assignment/create_gk.cpp
        assignment/create_mids.cpp
        assignment/create_static.cpp

        navigation/obstacle/map.cpp
        navigation/planner/planner.cpp
        navigation/trajectory/trajectory_bang_bang.cpp

        helpers/attack_fucking_angle.cpp
        helpers/ball_is_goaling.cpp
        helpers/ball_prediction.cpp
        helpers/calc_is_defending.cpp
        helpers/calculate_mark_cost.cpp
        helpers/calculate_opp_threat.cpp
        helpers/field.inl
        helpers/find_kicker_opp.cpp
        helpers/goal_blocked.cpp
        helpers/mark_manager.cpp
        helpers/nearest_opp.cpp
        helpers/open_angle.cpp
        helpers/strategy_weight.cpp

        plays/corner_their_global.cpp
        plays/halt.cpp
        plays/kickoff_their_one_wall.cpp
        plays/kickoff_us_chip.cpp
        plays/normal_play_att.cpp
        plays/normal_play_def.cpp
        plays/normal_play.cpp
        plays/penalty_their_simple.cpp
        plays/penalty_us_shootout.cpp
        plays/place_ball.cpp
        plays/stop.cpp
        plays/strategy.cpp
        plays/their_place_ball.cpp
        plays/throwin_chip_shoot.cpp

        skills/circle_ball.cpp
        skills/def_hi.cpp
        skills/defence_wall.cpp
        skills/gk_hi.cpp
        skills/halt.cpp
        skills/mark_to_ball.cpp
        skills/mark_to_goal.cpp
        skills/wait_for_pass.cpp
        skills/running_def.cpp

        robot/robot.cpp
        robot/navigation.cpp
        robot/set_obstacles.cpp

        tactics/attacker.cpp
        tactics/mark.cpp
        tactics/receive_pass.cpp

        zone/pos_attack.cpp
        zone/pos_defence.cpp
        zone/zone.cpp)

add_library(${PROJECT_NAME}
        ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC
        ${CMAKE_PROJECT_NAME}-common
        ${CMAKE_PROJECT_NAME}-sender
        unofficial::lemon::lemon)

set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ${USE_UNITY_BUILDS})

get_target_property(COMMON_PCHS ${CMAKE_PROJECT_NAME}-common INTERFACE_PRECOMPILE_HEADERS)
get_target_property(SENDER_PCHS ${CMAKE_PROJECT_NAME}-sender INTERFACE_PRECOMPILE_HEADERS)

target_precompile_headers(${PROJECT_NAME}
        PUBLIC ${COMMON_PCHS}
        PUBLIC ${SENDER_PCHS}
        PUBLIC pch.h
        INTERFACE ${HEADER_FILES})
