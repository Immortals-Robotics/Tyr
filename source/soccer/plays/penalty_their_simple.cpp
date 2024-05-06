#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::penalty_their_simple()
{
    float penalty_x = Common::field().width - 85.0;

    static VelocityProfile VELOCITY_PROFILE_KILLER = VELOCITY_PROFILE_KHARAKI;
    VELOCITY_PROFILE_KILLER.max_spd                = Common::Vec2(40.0f);
    VELOCITY_PROFILE_KILLER.max_dec                = Common::Vec2(1.8f);
    VELOCITY_PROFILE_KILLER.max_acc                = Common::Vec2(1.3f);

    int index = findKickerOpp(-1);
    if (index == -1)
    {
        OwnRobot[gk].target.angle = Common::Angle::fromDeg((1 + side) * 90.0f);
        Navigate2Point(gk, Common::Vec2(side * penalty_x));
    }
    else
    {
        //		float gkp_y = Common::Line::fromTwoPoints ( Common::Vec2 ( Common::worldState().opp_robot[index].position.x ,
        // Common::worldState().opp_robot[index].position.y ) , Common::worldState().ball.position).intersect (
        // Common::Line::fromTwoPoints ( Common::Vec2 ( side * penalty_x , 100 ) , Common::Vec2 ( side * penalty_x ,
        // -100 ) ) ).getY ( );
        // TODO #7 check this!!!!
        float gkp_y = Common::Line::fromPointAndAngle(Common::worldState().ball.position, Common::worldState().opp_robot[index].angle)
                          .intersect(Common::Line::fromTwoPoints(Common::Vec2(side * penalty_x, 100),
                                                                 Common::Vec2(side * penalty_x, -100)))
                          .value_or(Common::Vec2())
                          .y;
        float max_reach_y = (Common::field().goal_width / 2.0) - 50.0;
        if (max_reach_y < gkp_y)
            gkp_y = max_reach_y;
        if (-max_reach_y > gkp_y)
            gkp_y = -max_reach_y;

        OwnRobot[gk].face(Common::worldState().ball.position);

        Navigate2Point(gk, Common::Vec2(side * penalty_x, gkp_y), 100, &VELOCITY_PROFILE_KILLER);
    }
    ERRTSetObstacles(lw, true, true);
    ERRTNavigate2Point(lw, Common::Vec2(-side * 4300, 500), 80, &VELOCITY_PROFILE_AROOM);
    ERRTSetObstacles(rw, true, true);
    ERRTNavigate2Point(rw, Common::Vec2(-side * 4300, -500), 80, &VELOCITY_PROFILE_AROOM);
    ERRTSetObstacles(def, true, true);
    ERRTNavigate2Point(def, Common::Vec2(-side * 4300, 800), 80, &VELOCITY_PROFILE_AROOM);
    ERRTSetObstacles(dmf, true, true);
    ERRTNavigate2Point(dmf, Common::Vec2(-side * 4300, -800), 80, &VELOCITY_PROFILE_AROOM);
    ERRTSetObstacles(mid1, true, true);
    ERRTNavigate2Point(mid1, Common::Vec2(-side * 4300, -1500), 80, &VELOCITY_PROFILE_AROOM);
    ERRTSetObstacles(mid2, true, true);
    ERRTNavigate2Point(mid2, Common::Vec2(-side * 4300, 1500), 80, &VELOCITY_PROFILE_AROOM);
    ERRTSetObstacles(attack, true, true);
    ERRTNavigate2Point(attack, Common::Vec2(-side * 4300, 0), 80, &VELOCITY_PROFILE_AROOM);
}
} // namespace Tyr::Soccer