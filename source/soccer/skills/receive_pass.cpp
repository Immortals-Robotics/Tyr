#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::receivePass(Robot& t_robot, Common::Vec2 t_static_pos, const bool t_chip)
{
    const float contStrStaticTime = 1.0f;

    if (m_timer.time().seconds() > 0.7)
    {
        m_chip_head.setDeg(200);
    }

    const auto &allaf_pos = std::find_if(m_allaf_pos.begin(), m_allaf_pos.end(),
                                         [&t_robot](const auto &entry) { return *entry.first == t_robot.state().vision_id; });

    if (t_robot.one_touch_type == Common::Soccer::OneTouchType::Allaf &&
        allaf_pos != m_allaf_pos.end())
        t_static_pos = allaf_pos->second;
    if (t_robot.one_touch_type == Common::Soccer::OneTouchType::OneTouch &&
        m_timer.time().seconds() < contStrStaticTime && allaf_pos != m_allaf_pos.end())
        t_static_pos = allaf_pos->second;

    if (m_timer.time().seconds() > 2.5)
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

    float distCoeff = m_world_state.ball.position.distanceTo(t_robot.state().position) / 1500.0f;
    distCoeff       = std::max(0.8f, distCoeff);
    distCoeff       = std::min(1.2f, distCoeff);

    if ((t_robot.one_touch_detector.isArriving(angleTol, maxBallAngle)) &&
        (t_robot.one_touch_type != Common::Soccer::OneTouchType::Allaf))
    {
        Common::logDebug("robot {} one touch detected", t_robot.state().vision_id);

        t_robot.one_touch_type_used = true;
        if (t_robot.one_touch_type == Common::Soccer::OneTouchType::OneTouch)
        {
            if (m_timer.time().seconds() < contStrStaticTime)
                waitForPass(t_robot, t_chip, nullptr, &t_static_pos);
            else
                waitForPass(t_robot, t_chip);
        }
        else if (t_robot.one_touch_type == Common::Soccer::OneTouchType::Shirje)
        {
            waitForOmghi(t_robot);
        }
        else if (t_robot.one_touch_type == Common::Soccer::OneTouchType::Gool)
        {
            waitForGool(t_robot);
        }
        else
        { // probably Allaf!!!
            t_robot.face(oppGoal());
            t_robot.navigate(t_static_pos, VelocityProfile::mamooli());
        }
    }
    else
    {
        if (t_robot.one_touch_type_used)
        {
            t_robot.one_touch_type_used = false;
            t_robot.one_touch_type      = Common::Soccer::OneTouchType::OneTouch;
        }
        t_robot.face(oppGoal());
        t_robot.navigate(t_static_pos, VelocityProfile::mamooli(), NavigationFlags::ForceBallObstacle);
    }
}
} // namespace Tyr::Soccer
