#include "wait_for_ball.h"

#include "../helpers/ball_prediction.h"
#include "../robot/robot.h"

namespace Tyr::Soccer
{
const Skill::Id WaitForBallSkill::kId = &WaitForBallSkill::kId;

void WaitForBallSkill::execute(Robot &t_robot)
{
    const Common::BallState &ball         = State::world().ball;
    const VelocityProfile    profile      = VelocityProfile::mamooli();
    Common::Vec2             receiver_pos = m_static_pos;

    if (ball.velocity.length() >= 100)
    {
        if (m_mode == WaitMode::Quickest)
        {
            const Common::Angle ball_dir       = Common::Angle::fromVec(ball.velocity);
            const float         t_interception = calculateBallRobotReachTime(t_robot, ball_dir, profile, 1.0f);
            receiver_pos                       = predictBall(t_interception).position;
        }
        else
        {
            receiver_pos =
                Common::Line::fromTwoPoints(ball.position,
                                            ball.velocity.normalized() * 1000 + ball.position)
                    .closestPoint(t_robot.state().position);
        }
    }

    t_robot.navigate(receiver_pos, profile);

    t_robot.target.angle = receiver_pos.angleWith(ball.position);
}
} // namespace Tyr::Soccer
