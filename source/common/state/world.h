#pragma once

#include "../math/angle.h"
#include "../math/vector.h"
#include "../setting.h"

namespace Tyr::Common
{
enum class SeenState
{
    CompletelyOut = 0,
    TemprolilyOut = 1,
    Seen          = 2,
};

struct RobotState
{
    int       vision_id;
    TeamColor color;

    Vec2 position;
    Vec2 velocity;

    Angle angle;
    Angle angular_velocity;

    SeenState seen_state         = SeenState::CompletelyOut;
    bool      out_for_substitute = true;

    RobotState() = default;

    RobotState(const Protos::Immortals::RobotState &t_robot)
    {
        vision_id = t_robot.id();
        color     = (TeamColor) t_robot.color();

        position = t_robot.position();
        velocity = t_robot.velocity();

        angle            = t_robot.angle();
        angular_velocity = t_robot.angular_velocity();

        seen_state = (SeenState) t_robot.seen_state();
    }

    void fillProto(Protos::Immortals::RobotState *const t_robot) const
    {
        t_robot->set_id(vision_id);
        t_robot->set_color((Protos::Immortals::TeamColor) color);

        position.fillProto(t_robot->mutable_position());
        velocity.fillProto(t_robot->mutable_velocity());

        angle.fillProto(t_robot->mutable_angle());
        angular_velocity.fillProto(t_robot->mutable_angular_velocity());

        t_robot->set_seen_state((Protos::Immortals::SeenState) seen_state);
    }
};

struct BallState
{
    Vec2 position;
    Vec2 velocity;

    SeenState seen_state = SeenState::CompletelyOut;

    BallState() = default;

    BallState(const Protos::Immortals::BallState &t_ball)
    {
        position = t_ball.position();
        velocity = t_ball.velocity();

        seen_state = (SeenState) t_ball.seen_state();
    }

    void fillProto(Protos::Immortals::BallState *const t_ball) const
    {
        position.fillProto(t_ball->mutable_position());
        velocity.fillProto(t_ball->mutable_velocity());

        t_ball->set_seen_state((Protos::Immortals::SeenState) seen_state);
    }
};

struct FieldState
{
    float width  = 6000.0f;
    float height = 4500.0f;

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
