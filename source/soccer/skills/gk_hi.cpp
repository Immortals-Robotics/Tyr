#include "../ai.h"

namespace Tyr::Soccer
{
static int my_hys = 0;

void Ai::gkHi(const int t_robot_num)
{
    m_gk_intercepting = false;

    Common::logDebug("GKhi: {} _ {}", ballIsGoaling(),
                     m_world_state.ball.position.distanceTo(m_own_robot[t_robot_num].state().position) /
                         m_world_state.ball.velocity.length());
    if (ballIsGoaling())
    {
        Common::debug().draw(Common::Circle{m_own_robot[t_robot_num].state().position, 100}, Common::Color::red(),
                             false);
    }
    else
    {
        Common::debug().draw(Common::Circle{m_own_robot[t_robot_num].state().position, 100}, Common::Color::yellow(),
                             false);
    }

    if ((ballIsGoaling()) &&
        (m_world_state.ball.position.distanceTo(m_own_robot[t_robot_num].state().position) /
             m_world_state.ball.velocity.length() <
         3) &&
        m_ref_state.gameOn())
    {
        gkShirje(t_robot_num);
        my_hys = 10;
    }
    else if ((my_hys > 0) && m_ref_state.gameOn())
    {
        gkShirje(t_robot_num);
        my_hys--;
    }

    else
    {
        my_hys = 0;

        ObstacleMap obs_map;

        // our penalty area
        static constexpr float area_extension_size     = 200.0f;
        const float            penalty_area_half_width = Common::field().penalty_area_width / 2.0f;

        const Common::Vec2 start{ownGoal().x, -(penalty_area_half_width + area_extension_size)};

        const float w = -m_side * (area_extension_size + Common::field().penalty_area_depth);
        const float h = Common::field().penalty_area_width + 2 * area_extension_size;

        obs_map.addRectangle({start, w, h});

        if ((obs_map.isInObstacle(m_world_state.ball.position)) && (m_world_state.ball.velocity.length() < 1500) &&
            m_ref_state.gameOn())
        {
            Common::logDebug("GK intercepting");

            m_gk_intercepting = true;

            attacker(t_robot_num,
                     m_world_state.ball.position.angleWith(Common::Vec2(m_side * (Common::field().width + 110), 0)), 0,
                     80, 0, 0);
        }
        else
        {

            Common::Vec2 target = ownGoal().pointOnConnectingLine(m_world_state.ball.position, 1500);
            target.x            = Common::sign(target.x) * std::min(Common::field().width - 90, std::fabs(target.x));

            m_own_robot[t_robot_num].face(m_world_state.ball.position);
            navigate(t_robot_num, target, VelocityProfile::mamooli());
        }
    }
}

void Ai::gkShirje(const int t_robot_num)
{
    Common::logDebug("GK Shirje");

    Common::Line ball_line =
        Common::Line::fromPointAndAngle(m_world_state.ball.position, m_world_state.ball.velocity.toAngle());
    Common::Vec2 ans = ball_line.closestPoint(m_own_robot[t_robot_num].state().position);
    m_own_robot[t_robot_num].face(m_world_state.ball.position);
    ans = ((ans - m_own_robot[t_robot_num].state().position) * 2.0f) + m_own_robot[t_robot_num].state().position;
    navigate(t_robot_num, ans, VelocityProfile::kharaki());
    m_own_robot[t_robot_num].chip(150);
}
} // namespace Tyr::Soccer
