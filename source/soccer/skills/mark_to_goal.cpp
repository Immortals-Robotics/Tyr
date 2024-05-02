#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::Mark2Goal(int robot_num, int opp, float dist)
{
    const float opp_predict_t = 0.15;
    dist                      = std::min(1500.0f, dist);

    Common::Vec2 predictedOpp = Common::Vec2(OppRobot[opp].position.x + OppRobot[opp].velocity.x * opp_predict_t,
                                             OppRobot[opp].position.y + OppRobot[opp].velocity.y * opp_predict_t);
    Common::Vec2 target       = predictedOpp.pointOnConnectingLine(Common::Vec2(side * field_width, 0), dist);

    OwnRobot[robot_num].face(ball.position);
    ERRTSetObstacles(robot_num, 1, 1);
    ERRTNavigate2Point(robot_num, target, 0, 100,
                       refereeState->stop() ? &VELOCITY_PROFILE_AROOM : &VELOCITY_PROFILE_MAMOOLI);
}
} // namespace Tyr::Soccer
