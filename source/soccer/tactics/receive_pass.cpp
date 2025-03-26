#include "receive_pass.h"

#include "../robot/robot.h"

namespace Tyr::Soccer
{
const Tactic::Id ReceivePassTactic::kId = &ReceivePassTactic::kId;

void ReceivePassTactic::execute(Robot &t_robot)
{
    t_robot.face(State::world().ball.position);
    t_robot.navigate(m_static_pos, VelocityProfile::mamooli(), NavigationFlags::BallObstacle);
}
} // namespace Tyr::Soccer
