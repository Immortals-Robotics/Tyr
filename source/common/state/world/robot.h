#pragma once

#include "seen_state.h"

namespace Immortals::Common
{
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

    explicit RobotState(const Protos::Immortals::RobotState &t_robot)
    {
        vision_id = t_robot.id();
        color     = static_cast<TeamColor>(t_robot.color());

        position = Vec2{t_robot.position()};
        velocity = Vec2{t_robot.velocity()};

        angle            = Angle{t_robot.angle()};
        angular_velocity = Angle{t_robot.angular_velocity()};

        seen_state         = static_cast<SeenState>(t_robot.seen_state());
        out_for_substitute = t_robot.out_for_substitute();
    }

    void fillProto(Protos::Immortals::RobotState *const t_robot) const
    {
        t_robot->set_id(vision_id);
        t_robot->set_color(static_cast<Protos::Immortals::TeamColor>(color));

        position.fillProto(t_robot->mutable_position());
        velocity.fillProto(t_robot->mutable_velocity());

        angle.fillProto(t_robot->mutable_angle());
        angular_velocity.fillProto(t_robot->mutable_angular_velocity());

        t_robot->set_seen_state(static_cast<Protos::Immortals::SeenState>(seen_state));
        t_robot->set_out_for_substitute(out_for_substitute);
    }
};
} // namespace Immortals::Common
