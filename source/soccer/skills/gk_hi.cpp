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
    // side = -side;
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
        // OwnRobot[robot_num].Chip(50);

        my_hys = 0;

        ERRTSetGkClearObstacles(robot_num);
        if ((g_obs_map.isInObstacle(m_world_state.ball.position)) && (m_world_state.ball.velocity.length() < 1500) &&
            m_ref_state.canKickBall() && (!stop))
        {
            Common::logDebug("GK intercepting");

            gkIntercepting = true;

            ERRTSetObstacles(robot_num);
            // tech_circle(robot_num,Common::sign(m_world_state.ball.position.y)*side*60 ,0,15,false);
            tech_circle(robot_num,
                        m_world_state.ball.position.angleWith(Common::Vec2(side * (Common::field().width + 110), 0)), 0,
                        80, false, 0, 0, 0);
        }
        else
        {
#if 0

            float cornerStartMul = pow(std::max(0,1.2-timer.time()),1.1);

			//Added by Dot_blue:
			float penalty_x = Common::field().width - 85.0;
			float max_reach_y = (Common::field().goal_width/2.0) + 20.0;
			float tmp_x,tmp_y;
			Common::Line GK_Rail = Common::Line::fromTwoPoints(Common::Vec2(side * penalty_x, 100),
													   Common::Vec2(side * penalty_x, -100));
			Common::Vec2 ans;
			if(DIS(m_world_state.ball.position,Common::Vec2(side*Common::field().width,0))<2500 && m_world_state.ball.position.x > side*(Common::field().width -1200) ){
				ans = GK_Rail.closestPoint(m_world_state.ball.position);
				tmp_x = ans.getX();
				tmp_y = ans.getY();
			} else {
				tmp_x = side * penalty_x;
				tmp_y = (6.0 * m_world_state.ball.position.y) / 40.0f;
			}

			if (tmp_y < -max_reach_y) {
				tmp_y = -max_reach_y;
			}
			if (tmp_y > max_reach_y) {
				tmp_y = max_reach_y;
			}
			Common::Vec2 target = Common::Vec2(tmp_x, tmp_y);
			//Done by Dot_Blue TODO #9 test this...
#else
            Common::Vec2 target =
                Common::Vec2(side * Common::field().width, 0).pointOnConnectingLine(m_world_state.ball.position, 1500);
            target.x = Common::sign(target.x) * std::min(Common::field().width - 90, std::fabs(target.x));
#endif
            OwnRobot[robot_num].face(m_world_state.ball.position);
            ERRTSetObstacles(robot_num);
            ERRTNavigate2Point(robot_num, target, 80,
                               stop ? VelocityProfile::Type::Aroom : VelocityProfile::Type::Mamooli);
        }
    }
    // side = -side;
}

void Ai::GK_shirje(int robot_num)
{
    Common::logDebug("GK shirje");

    Common::Line ball_line =
        Common::Line::fromPointAndAngle(m_world_state.ball.position, m_world_state.ball.velocity.toAngle());
    Common::Vec2 ans = ball_line.closestPoint(
        Common::Vec2(OwnRobot[robot_num].state().position.x, OwnRobot[robot_num].state().position.y));
    OwnRobot[robot_num].face(m_world_state.ball.position);
    ans = ((ans - OwnRobot[robot_num].state().position) * 2.0f) + OwnRobot[robot_num].state().position;
    ERRTSetObstacles(robot_num);
    ERRTNavigate2Point(robot_num, ans, 100, VelocityProfile::Type::Kharaki);
    OwnRobot[robot_num].Chip(150);
}
} // namespace Tyr::Soccer
