#include "receive_pass.h"

#include "../robot/robot.h"

#include "../skills/one_touch.h"

namespace Tyr::Soccer
{
const Tactic::Id ReceivePassTactic::kId = &ReceivePassTactic::kId;

void ReceivePassTactic::execute(Robot &t_robot)
{
    if (State::timer().time().seconds() > 2.5)
    {
        t_robot.one_touch_type_used = true;
    }

    float angleTol, maxBallAngle;

    if (t_robot.one_touch_type == Common::Soccer::OneTouchType::OneTouch)
    {
        angleTol     = 45;
        maxBallAngle = 150;
    }
    else if (t_robot.one_touch_type == Common::Soccer::OneTouchType::Shirje)
    {
        // angleTol = 55;
        // maxBallAngle = 140;
        angleTol     = 45;
        maxBallAngle = 90;
    }
    else if (t_robot.one_touch_type == Common::Soccer::OneTouchType::Gool)
    {
        angleTol     = 35;
        maxBallAngle = 110;
    }
    else
    { // probably Allaf!!!
        angleTol     = 0;
        maxBallAngle = 0;
    }

    float distCoeff = State::world().ball.position.distanceTo(t_robot.state().position) / 1500.0f;
    distCoeff       = std::max(0.8f, distCoeff);
    distCoeff       = std::min(1.2f, distCoeff);

    if ((t_robot.one_touch_detector.isArriving(angleTol, maxBallAngle)) &&
        (t_robot.one_touch_type != Common::Soccer::OneTouchType::Allaf))
    {
        Common::logDebug("robot {} one touch detected", t_robot.state().vision_id);

        t_robot.one_touch_type_used = true;
        if (t_robot.one_touch_type == Common::Soccer::OneTouchType::OneTouch)
        {
            OneTouchSkill{m_chip}.execute(t_robot);
        }
        else if (t_robot.one_touch_type == Common::Soccer::OneTouchType::Allaf)
        {
            t_robot.face(Field::oppGoal());
            t_robot.navigate(m_static_pos, VelocityProfile::mamooli());
        }
        else
        {
            Common::logWarning("robot {} has unsupported one-touch type {}", t_robot.state().vision_id,
                               t_robot.one_touch_type);
        }
    }
    else
    {
        if (t_robot.one_touch_type_used)
        {
            t_robot.one_touch_type_used = false;
            t_robot.one_touch_type      = Common::Soccer::OneTouchType::OneTouch;
        }
        t_robot.face(Field::oppGoal());
        t_robot.navigate(m_static_pos, VelocityProfile::mamooli(), NavigationFlags::BallObstacle);
    }
}
} // namespace Tyr::Soccer
