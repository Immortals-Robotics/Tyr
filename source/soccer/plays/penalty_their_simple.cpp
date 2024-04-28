#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::penalty_their_simple(void)
{
    float penalty_x = field_width - 85.0;

    static VelocityProfile VELOCITY_PROFILE_KILLER = VELOCITY_PROFILE_KHARAKI;
    VELOCITY_PROFILE_KILLER.max_spd                = Common::Vec2(40.0f);
    VELOCITY_PROFILE_KILLER.max_dec                = Common::Vec2(1.8f);
    VELOCITY_PROFILE_KILLER.max_acc                = Common::Vec2(1.3f);

    int index = findKickerOpp(-1);
    if (index == -1)
    {
        OwnRobot[gk].target.angle = Common::Angle::fromDeg((1 + side) * 90.0f);
        Navigate2Point(gk, Common::Vec2(side * penalty_x, 0));
    }
    else
    {
        //		float gkp_y = Line::makeLineFromTwoPoints ( VecPosition ( OppRobot[index].Position.x ,
        // OppRobot[index].Position.y ) , VecPosition ( ball.Position.x , ball.Position.y ) ).getIntersection (
        // Line::makeLineFromTwoPoints ( VecPosition ( side * penalty_x , 100 ) , VecPosition ( side * penalty_x , -100
        // ) ) ).getY ( );
        // TODO #7 check this!!!!
        float gkp_y =
            Line::makeLineFromPositionAndAngle(VecPosition(ball.Position.x, ball.Position.y), OppRobot[index].Angle)
                .getIntersection(Line::makeLineFromTwoPoints(VecPosition(side * penalty_x, 100),
                                                             VecPosition(side * penalty_x, -100)))
                .getY();
        float max_reach_y = (goal_width / 2.0) - 50.0;
        if (max_reach_y < gkp_y)
            gkp_y = max_reach_y;
        if (-max_reach_y > gkp_y)
            gkp_y = -max_reach_y;

        OwnRobot[gk].face(ball.Position);

        Navigate2Point(gk, Common::Vec2(side * penalty_x, gkp_y), false, 100, &VELOCITY_PROFILE_KILLER);
    }
    ERRTSetObstacles(lw, true, true);
    ERRTNavigate2Point(lw, Common::Vec2(-side * 4300, 500), false, 80, &VELOCITY_PROFILE_AROOM);
    ERRTSetObstacles(rw, true, true);
    ERRTNavigate2Point(rw, Common::Vec2(-side * 4300, -500), false, 80, &VELOCITY_PROFILE_AROOM);
    ERRTSetObstacles(def, true, true);
    ERRTNavigate2Point(def, Common::Vec2(-side * 4300, 800), false, 80, &VELOCITY_PROFILE_AROOM);
    ERRTSetObstacles(dmf, true, true);
    ERRTNavigate2Point(dmf, Common::Vec2(-side * 4300, -800), false, 80, &VELOCITY_PROFILE_AROOM);
    ERRTSetObstacles(mid1, true, true);
    ERRTNavigate2Point(mid1, Common::Vec2(-side * 4300, -1500), false, 80, &VELOCITY_PROFILE_AROOM);
    ERRTSetObstacles(mid2, true, true);
    ERRTNavigate2Point(mid2, Common::Vec2(-side * 4300, 1500), false, 80, &VELOCITY_PROFILE_AROOM);
    ERRTSetObstacles(attack, true, true);
    ERRTNavigate2Point(attack, Common::Vec2(-side * 4300, 0), false, 80, &VELOCITY_PROFILE_AROOM);
}
} // namespace Tyr::Soccer