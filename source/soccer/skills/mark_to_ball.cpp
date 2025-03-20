#include "mark_to_ball.h"

#include "../robot/robot.h"

namespace Tyr::Soccer
{
const Skill::Id MarkToBallSkill::kId = &MarkToBallSkill::kId;

void MarkToBallSkill::execute(Robot &t_robot)
{
    const float opp_predict_t = 0.15f;

    Common::Vec2 predictedOpp = m_opp.position + m_opp.velocity * opp_predict_t;
    Common::Vec2 target       = predictedOpp.pointOnConnectingLine(State::world().ball.position, m_dist);

    t_robot.face(State::world().ball.position);
    t_robot.navigate(target, VelocityProfile::mamooli(), NavigationFlags::ForceBallObstacle);
}

} // namespace Tyr::Soccer
