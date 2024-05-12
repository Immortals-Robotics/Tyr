#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::penalty_their_simple()
{
    float penalty_x = Common::field().width - 85.0;

    int index = findKickerOpp(-1);
    if (index == -1)
    {
        OwnRobot[gk].target.angle = Common::Angle::fromDeg((1 + side) * 90.0f);
        Navigate2Point(gk, Common::Vec2(side * penalty_x));
    }
    else
    {
        //		float gkp_y = Common::Line::fromTwoPoints ( Common::Vec2 (
        //m_world_state.opp_robot[index].position.x ,
        // m_world_state.opp_robot[index].position.y ) , m_world_state.ball.position).intersect (
        // Common::Line::fromTwoPoints ( Common::Vec2 ( side * penalty_x , 100 ) , Common::Vec2 ( side * penalty_x ,
        // -100 ) ) ).getY ( );
        // TODO #7 check this!!!!
        float gkp_y = Common::Line::fromPointAndAngle(m_world_state.ball.position,
                                                      m_world_state.opp_robot[index].angle)
                          .intersect(Common::Line::fromTwoPoints(Common::Vec2(side * penalty_x, 100),
                                                                 Common::Vec2(side * penalty_x, -100)))
                          .value_or(Common::Vec2())
                          .y;
        float max_reach_y = (Common::field().goal_width / 2.0) - 50.0;
        if (max_reach_y < gkp_y)
            gkp_y = max_reach_y;
        if (-max_reach_y > gkp_y)
            gkp_y = -max_reach_y;

        OwnRobot[gk].face(m_world_state.ball.position);

        Navigate2Point(gk, Common::Vec2(side * penalty_x, gkp_y), 100, VelocityProfile::Type::Killer);
    }
    ERRTSetObstacles(lw, true, true);
    ERRTNavigate2Point(lw, Common::Vec2(-side * 4300, 500), 80, VelocityProfile::Type::Aroom);
    ERRTSetObstacles(rw, true, true);
    ERRTNavigate2Point(rw, Common::Vec2(-side * 4300, -500), 80, VelocityProfile::Type::Aroom);
    ERRTSetObstacles(def, true, true);
    ERRTNavigate2Point(def, Common::Vec2(-side * 4300, 800), 80, VelocityProfile::Type::Aroom);
    ERRTSetObstacles(dmf, true, true);
    ERRTNavigate2Point(dmf, Common::Vec2(-side * 4300, -800), 80, VelocityProfile::Type::Aroom);
    ERRTSetObstacles(mid1, true, true);
    ERRTNavigate2Point(mid1, Common::Vec2(-side * 4300, -1500), 80, VelocityProfile::Type::Aroom);
    ERRTSetObstacles(mid2, true, true);
    ERRTNavigate2Point(mid2, Common::Vec2(-side * 4300, 1500), 80, VelocityProfile::Type::Aroom);
    ERRTSetObstacles(attack, true, true);
    ERRTNavigate2Point(attack, Common::Vec2(-side * 4300, 0), 80, VelocityProfile::Type::Aroom);
}
} // namespace Tyr::Soccer