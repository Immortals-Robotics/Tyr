#include "../ai.h"

namespace Tyr::Soccer
{
static int my_hys = 0;

void Ai::GKHi(int robot_num, bool stop)
{
    gkIntercepting = false;

    LOG_DEBUG("GKhi: {} _ {}", ballIsGoaling(),
              ball.Position.distanceTo(OwnRobot[robot_num].State.Position) / ball.velocity.length());
    // side = -side;
    debugDraw = true; // TODO #10 Comment this in the game
    if (ballIsGoaling())
    {
        Common::debug().drawCircle(OwnRobot[robot_num].State.Position, 200, "", Common::Red);
    }
    else
    {
        Common::debug().drawCircle(OwnRobot[robot_num].State.Position, 20, "", Common::Yellow);
    }
    debugDraw = false;

    if ((ballIsGoaling()) &&
        (ball.Position.distanceTo(OwnRobot[robot_num].State.Position) / ball.velocity.length() < 3) && (!stop))
    {
        // WaitForPass ( robot_num , true );
        GK_shirje(robot_num);
        my_hys = 10;
    }
    else if ((my_hys > 0) && (!stop))
    {
        // WaitForPass ( robot_num , true );
        GK_shirje(robot_num);
        my_hys--;
    }

    else
    {
        // OwnRobot[robot_num].Chip(50);

        my_hys = 0;

        ERRTSetGkClearObstacles(robot_num);
        if ((IsInObstacle(ball.Position)) && (ball.velocity.length() < 1500) && REF_playState->canKickBall() && (!stop))
        {
            LOG_DEBUG("GK intercepting");

            gkIntercepting = true;

            ERRTSetObstacles(robot_num, 0, 0);
            // tech_circle(robot_num,Common::sign(ball.Position.y)*side*60 ,0,15,false);
            tech_circle(robot_num, ball.Position.angleWith(Common::Vec2(side * (field_width + 110), 0)), 0, 80, false,
                        0, 0, 0);
        }
        else
        {
#if 0

            float cornerStartMul = pow(std::max(0,1.2-timer.time()),1.1);
//			Common::Vec2 target = GK_Ghuz_2018(cornerStartMul*0.4, cornerStartMul>0?(1+0.2*(1-cornerStartMul)):1,1);
//			Common::Vec2 target = GK_Ghuz(cornerStartMul*0.4, cornerStartMul>0?(1+0.2*(1-cornerStartMul)):1,1);

			//Added by Dot_blue:
			float penalty_x = field_width - 85.0;
			float max_reach_y = (goal_width/2.0) + 20.0;
			float tmp_x,tmp_y;
			Common::Line GK_Rail = Common::Line::fromTwoPoints(Common::Vec2(side * penalty_x, 100),
													   Common::Vec2(side * penalty_x, -100));
			Common::Vec2 ans;
			if(DIS(ball.Position,Common::Vec2(side*field_width,0))<2500 && ball.Position.x > side*(field_width -1200) ){
				ans = GK_Rail.closestPoint(ball.Position);
				tmp_x = ans.getX();
				tmp_y = ans.getY();
			} else {
				tmp_x = side * penalty_x;
				tmp_y = (6.0 * ball.Position.y) / 40.0f;
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
            Common::Vec2 target = Common::Vec2(side * field_width, 0).pointOnConnectingLine(ball.Position, 1500);
            target.x            = Common::sign(target.x) * std::min(field_width - 90, std::fabs(target.x));
#endif
            OwnRobot[robot_num].face(ball.Position);
            ERRTSetObstacles(robot_num, stop, false);
            ERRTNavigate2Point(robot_num, target, 0, 80, stop ? &VELOCITY_PROFILE_AROOM : &VELOCITY_PROFILE_MAMOOLI);

            //			float R_robot = DIS(Common::Vec2(-field_width, 0),OwnRobot[robot_num].State.Position);
            //			float alpha_robot = AngleWith(Common::Vec2(-field_width, 0),OwnRobot[robot_num].State.Position);
            //			alpha_robot = std::min(90, std::max(-90, alpha_robot));
            //			std::cout<<"GOALIE teta: "<<alpha_robot<<std::endl;
            //			std::cout<<"GOALIE R: "<<R_robot<<std::endl;
            //			std::cout<<"______________________"<<std::endl;
        }
    }
    // side = -side;
}

void Ai::GK_shirje(int robot_num)
{
    LOG_DEBUG("GK shirje");

    Common::Line ball_line =
        Common::Line::fromPointAndAngle(ball.Position, ball.velocity.toAngle());
    Common::Vec2 ans = ball_line.closestPoint(
        Common::Vec2(OwnRobot[robot_num].State.Position.x, OwnRobot[robot_num].State.Position.y));
    OwnRobot[robot_num].face(ball.Position);
    ans = ((ans - OwnRobot[robot_num].State.Position) * 2.0f) + OwnRobot[robot_num].State.Position;
    ERRTSetObstacles(robot_num, 0, 0);
    ERRTNavigate2Point(robot_num, ans, 1, 100, &VELOCITY_PROFILE_KHARAKI);
    OwnRobot[robot_num].Chip(150);
}
} // namespace Tyr::Soccer
