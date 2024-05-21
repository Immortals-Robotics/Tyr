#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::receivePass(int t_robot_num, Common::Vec2 t_static_pos, bool t_chip)
{
    const float contStrStaticTime = 1.0f;

    if (m_timer.time().seconds() > 0.7)
    {
        m_chip_head.setDeg(200);
    }
    if (m_one_touch_type[t_robot_num] == OneTouchType::Allaf)
        t_static_pos = m_allaf_pos[t_robot_num];
    if (m_one_touch_type[t_robot_num] == OneTouchType::OneTouch && m_timer.time().seconds() < contStrStaticTime)
        t_static_pos = m_allaf_pos[t_robot_num];

    if (m_timer.time().seconds() > 2.5)
    {
        m_one_touch_type_used[t_robot_num] = true;
    }

    float angleTol, maxBallAngle;

    if (m_one_touch_type[t_robot_num] == OneTouchType::OneTouch)
    {
        angleTol     = 45;
        maxBallAngle = 150;
    }
    else if (m_one_touch_type[t_robot_num] == OneTouchType::Shirje)
    {
        // angleTol = 55;
        // maxBallAngle = 140;
        angleTol     = 45;
        maxBallAngle = 90;
    }
    else if (m_one_touch_type[t_robot_num] == OneTouchType::Gool)
    {
        angleTol     = 35;
        maxBallAngle = 110;
    }
    else
    { // probably Allaf!!!
        angleTol     = 0;
        maxBallAngle = 0;
    }

    float distCoeff = m_world_state.ball.position.distanceTo(m_own_robot[t_robot_num].state().position) / 1500.0f;
    distCoeff       = std::max(0.8f, distCoeff);
    distCoeff       = std::min(1.2f, distCoeff);

    if ((m_one_touch_detector[t_robot_num].IsArriving(angleTol, maxBallAngle)) &&
        (m_one_touch_type[t_robot_num] != OneTouchType::Allaf))
    {
        m_one_touch_type_used[t_robot_num] = true;
        if (m_one_touch_type[t_robot_num] == OneTouchType::OneTouch)
        {
            if (m_timer.time().seconds() < contStrStaticTime)
                waitForPass(t_robot_num, t_chip, nullptr, &t_static_pos);
            else
                waitForPass(t_robot_num, t_chip);
        }
        else if (m_one_touch_type[t_robot_num] == OneTouchType::Shirje)
        {
            waitForOmghi(t_robot_num, t_chip);
        }
        else if (m_one_touch_type[t_robot_num] == OneTouchType::Gool)
        {
            waitForGool(t_robot_num, t_chip);
        }
        else
        { // probably Allaf!!!
            m_own_robot[t_robot_num].face(oppGoal());
            navigate(t_robot_num, t_static_pos, VelocityProfile::mamooli());
        }
    }
    else
    {
        if (m_one_touch_type_used[t_robot_num])
        {
            m_one_touch_type_used[t_robot_num] = false;
            m_one_touch_type[t_robot_num]      = OneTouchType::OneTouch;
        }
        m_own_robot[t_robot_num].face(oppGoal());
        navigate(t_robot_num, t_static_pos, VelocityProfile::mamooli(), NavigationFlagsForceBallObstacle);
    }
}
} // namespace Tyr::Soccer
