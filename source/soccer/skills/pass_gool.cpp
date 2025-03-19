#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::waitForGool(Robot& t_robot)
{
    Common::Vec2 pos = calculatePassPos(oppGoal(), t_robot.state().position, -1600);

    t_robot.face(oppGoal());

    t_robot.navigate(pos, VelocityProfile::mamooli());

    //TODO: What is this number?
    t_robot.shoot(6500.f);
    t_robot.dribble(15);
}
} // namespace Tyr::Soccer
