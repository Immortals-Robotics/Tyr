#pragma once

#include "../math/angle.h"
#include "../math/vector.h"
#include "../setting.h"

namespace Tyr::Common
{
enum class SeenState
{
    Seen,
    CompletelyOut,
    TemprolilyOut,
};

struct RobotState
{
    int vision_id;

    Vec2 position;
    Vec2 velocity;

    Angle angle;
    Angle angular_velocity;

    SeenState seen_state         = SeenState::CompletelyOut;
    bool      out_for_substitute = true;
};

struct BallState
{
    Vec2 position;
    Vec2 velocity;

    SeenState seen_state = SeenState::CompletelyOut;
};

struct FieldState
{
    float width  = 12000.0f;
    float height = 9000.0f;

    float goal_width = 1800.0f;

    float boundary_width;

    float penalty_area_depth = 1800.0f;
    float penalty_area_width = 3600.0f;

    float ball_radius;
    float max_robot_radius;
};

struct WorldState
{
    BallState ball;

    RobotState own_robot[Setting::kMaxRobots];
    RobotState opp_robot[Setting::kMaxRobots];

    FieldState field;

    Vec3 last_cmds[Setting::kMaxRobots][11] = {};

    WorldState()
    {
        for (int i = 0; i < Setting::kMaxRobots; i++)
        {
            own_robot[i].vision_id = i;
        }
        for (int i = 0; i < Setting::kMaxRobots; i++)
        {
            opp_robot[i].vision_id = i;
        }
    }
};
} // namespace Tyr::Common
