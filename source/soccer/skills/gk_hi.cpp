#include "../ai.h"

namespace Tyr::Soccer
{
static int my_hys = 0;

void Ai::GKHi(int robot_num, bool stop)
{
    gkIntercepting = false;

    Common::logDebug("GKhi: {} _ {}", ballIsGoaling(),
                     m_world_state.ball.position.distanceTo(OwnRobot[robot_num].state().position) /
                         m_world_state.ball.velocity.length());
    if (ballIsGoaling())
    {
        Common::debug().draw(Common::Circle{OwnRobot[robot_num].state().position, 100}, Common::Color::red(), false);
    }
    else
    {
        Common::debug().draw(Common::Circle{OwnRobot[robot_num].state().position, 100}, Common::Color::yellow(), false);
    }

    if ((ballIsGoaling()) &&
        (m_world_state.ball.position.distanceTo(OwnRobot[robot_num].state().position) /
             m_world_state.ball.velocity.length() <
         3) &&
        (!stop))
    {
        GK_shirje(robot_num);
        my_hys = 10;
    }
    else if ((my_hys > 0) && (!stop))
    {
        GK_shirje(robot_num);
        my_hys--;
    }

    else
    {
        // OwnRobot[robot_num].chip(50);

        my_hys = 0;

        ObstacleMap obs_map;

        // our penalty area
        static constexpr float area_extension_size     = 200.0f;
        const float            penalty_area_half_width = Common::field().penalty_area_width / 2.0f;

        const Common::Vec2 start{ownGoal().x, -(penalty_area_half_width + area_extension_size)};

        const float w = -side * (area_extension_size + Common::field().penalty_area_depth);
        const float h = Common::field().penalty_area_width + 2 * area_extension_size;

        obs_map.addRectangle({start, w, h});

        if ((obs_map.isInObstacle(m_world_state.ball.position)) && (m_world_state.ball.velocity.length() < 1500) &&
            m_ref_state.canKickBall() && (!stop))
        {
            Common::logDebug("GK intercepting");

            gkIntercepting = true;

            attacker(robot_num,
                        m_world_state.ball.position.angleWith(Common::Vec2(side * (Common::field().width + 110), 0)), 0,
                        80, 0, 0, 0);
        }
        else
        {

            Common::Vec2 target = ownGoal().pointOnConnectingLine(m_world_state.ball.position, 1500);
            target.x            = Common::sign(target.x) * std::min(Common::field().width - 90, std::fabs(target.x));

            OwnRobot[robot_num].face(m_world_state.ball.position);
            navigate(robot_num, target, VelocityProfile::mamooli());
        }
    }
    // side = -side;
}

void Ai::GK_shirje(int robot_num)
{
    Common::logDebug("GK shirje");

    Common::Line ball_line =
        Common::Line::fromPointAndAngle(m_world_state.ball.position, m_world_state.ball.velocity.toAngle());
    Common::Vec2 ans = ball_line.closestPoint(OwnRobot[robot_num].state().position);
    OwnRobot[robot_num].face(m_world_state.ball.position);
    ans = ((ans - OwnRobot[robot_num].state().position) * 2.0f) + OwnRobot[robot_num].state().position;
    navigate(robot_num, ans, VelocityProfile::kharaki());
    OwnRobot[robot_num].chip(150);
}
} // namespace Tyr::Soccer
