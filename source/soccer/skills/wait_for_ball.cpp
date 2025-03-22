#include "wait_for_ball.h"

#include "../robot/robot.h"

namespace Tyr::Soccer
{
const Skill::Id WaitForBallSkill::kId = &WaitForBallSkill::kId;

void WaitForBallSkill::execute(Robot &t_robot)
{
    Common::Vec2 receiver_pos = m_static_pos;

    if (State::world().ball.velocity.length() >= 100)
    {
        receiver_pos =
            Common::Line::fromTwoPoints(State::world().ball.position,
                                        State::world().ball.velocity.normalized() * 1000 + State::world().ball.position)
            .closestPoint(t_robot.state().position);
    }

    t_robot.navigate(receiver_pos, VelocityProfile::mamooli());

    t_robot.target.angle = receiver_pos.angleWith(State::world().ball.position);
}
} // namespace Tyr::Soccer
