#include "intercept_ball.h"

#include "../helpers/ball_prediction.h"
#include "../robot/robot.h"

namespace Tyr::Soccer
{
const Skill::Id InterceptBallSkill::kId = &InterceptBallSkill::kId;

void InterceptBallSkill::execute(Robot &t_robot)
{
    const Common::BallState &ball = State::world().ball;

    if (ball.velocity.length() < 100.0f)
    {
        t_robot.halt();
        return;
    }

    const VelocityProfile profile = VelocityProfile::mamooli();

    Common::debug().draw(
        Common::LineSegment{State::world().ball.position, State::world().ball.position + m_angle.toUnitVec() * 1000.0f},
        Common::Color::blue());

    const Common::Angle interception_angle = Common::Angle::fromVec(ball.velocity);
    const float         t_interception     = calculateBallRobotReachTime(t_robot, interception_angle, profile, m_wait_t);

    const Common::BallState ball_state = predictBall(t_interception);
    const Common::Vec2      interception_point =
        ball_state.position.circleAroundPoint(Common::Angle::fromVec(ball.velocity), Common::field().robot_radius);

    Common::debug().draw(interception_point);

    t_robot.navigate(interception_point, profile, NavigationFlags::BallSmallObstacle);

    t_robot.target.angle = Common::Angle::fromVec(ball_state.position - interception_point);
}
} // namespace Tyr::Soccer
