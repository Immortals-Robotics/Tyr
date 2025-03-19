#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::receivePass(const int t_robot_num, Common::Vec2 t_static_pos, const bool t_chip)
{
    const float contStrStaticTime = 1.0f;

    if (m_timer.time().seconds() > 0.7)
    {
        m_chip_head.setDeg(200);
    }

    const auto &allaf_pos = std::find_if(m_allaf_pos.begin(), m_allaf_pos.end(),
                                         [t_robot_num](const auto &entry) { return *entry.first == t_robot_num; });

    if (m_own_robot[t_robot_num].one_touch_type == Common::Soccer::OneTouchType::Allaf &&
        allaf_pos != m_allaf_pos.end())
        t_static_pos = allaf_pos->second;
    if (m_own_robot[t_robot_num].one_touch_type == Common::Soccer::OneTouchType::OneTouch &&
        m_timer.time().seconds() < contStrStaticTime && allaf_pos != m_allaf_pos.end())
        t_static_pos = allaf_pos->second;

    if (m_timer.time().seconds() > 2.5)
    {
        m_own_robot[t_robot_num].one_touch_type_used = true;
    }

    float angleTol, maxBallAngle;

    if (m_own_robot[t_robot_num].one_touch_type == Common::Soccer::OneTouchType::OneTouch)
    {
        angleTol     = 45;
        maxBallAngle = 150;
    }
    else if (m_own_robot[t_robot_num].one_touch_type == Common::Soccer::OneTouchType::Shirje)
    {
        // angleTol = 55;
        // maxBallAngle = 140;
        angleTol     = 45;
        maxBallAngle = 90;
    }
    else if (m_own_robot[t_robot_num].one_touch_type == Common::Soccer::OneTouchType::Gool)
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

    if ((m_own_robot[t_robot_num].one_touch_detector.isArriving(angleTol, maxBallAngle)) &&
        (m_own_robot[t_robot_num].one_touch_type != Common::Soccer::OneTouchType::Allaf))
    {
        Common::logDebug("robot {} one touch detected", t_robot_num);

        m_own_robot[t_robot_num].one_touch_type_used = true;
        if (m_own_robot[t_robot_num].one_touch_type == Common::Soccer::OneTouchType::OneTouch)
        {
            if (m_timer.time().seconds() < contStrStaticTime)
                waitForPass(t_robot_num, t_chip, nullptr, &t_static_pos);
            else
                waitForPass(t_robot_num, t_chip);
        }
        else if (m_own_robot[t_robot_num].one_touch_type == Common::Soccer::OneTouchType::Shirje)
        {
            waitForOmghi(t_robot_num);
        }
        else if (m_own_robot[t_robot_num].one_touch_type == Common::Soccer::OneTouchType::Gool)
        {
            waitForGool(t_robot_num);
        }
        else
        { // probably Allaf!!!
            m_own_robot[t_robot_num].face(oppGoal());
            m_own_robot[t_robot_num].navigate(t_static_pos, VelocityProfile::mamooli());
        }
    }
    else
    {
        if (m_own_robot[t_robot_num].one_touch_type_used)
        {
            m_own_robot[t_robot_num].one_touch_type_used = false;
            m_own_robot[t_robot_num].one_touch_type      = Common::Soccer::OneTouchType::OneTouch;
        }
        m_own_robot[t_robot_num].face(oppGoal());
        m_own_robot[t_robot_num].navigate(t_static_pos, VelocityProfile::mamooli(), NavigationFlags::ForceBallObstacle);
    }
}
} // namespace Tyr::Soccer
