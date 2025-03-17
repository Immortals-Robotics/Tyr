#pragma once

#include "seen_state.h"

namespace Immortals::Common
{
struct BallState
{
    Vec2 position;
    Vec2 velocity;

    SeenState seen_state = SeenState::CompletelyOut;

    BallState() = default;

    explicit BallState(const Protos::Immortals::BallState &t_ball)
    {
        position = Vec2{t_ball.position()};
        velocity = Vec2{t_ball.velocity()};

        seen_state = static_cast<SeenState>(t_ball.seen_state());
    }

    explicit BallState(const Protos::Ssl::Vision::DetectionBall &t_ball)
    {
        position   = Vec2(t_ball.x(), t_ball.y());
        seen_state = SeenState::Seen;
    }

    void fillProto(Protos::Immortals::BallState *const t_ball) const
    {
        position.fillProto(t_ball->mutable_position());
        velocity.fillProto(t_ball->mutable_velocity());

        t_ball->set_seen_state(static_cast<Protos::Immortals::SeenState>(seen_state));
    }

    Line line() const
    {
        return Line::fromPointAndAngle(position, velocity.toAngle());
    }
};
} // namespace Immortals::Common
