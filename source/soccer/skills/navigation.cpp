#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::navigate(int robot_num, Common::Vec2 dest, VelocityProfile::Type velocityProfile)
{
    if (OwnRobot[robot_num].state().seen_state == Common::SeenState::CompletelyOut)
        return;

    if (m_ref_state.shouldSlowDown() &&
        (velocityProfile == VelocityProfile::Type::Mamooli || velocityProfile == VelocityProfile::Type::Kharaki ||
         velocityProfile == VelocityProfile::Type::Killer))
    {
        velocityProfile = VelocityProfile::Type::Aroom;
    }

    if (OwnRobot[robot_num].state().position.distanceTo(dest) > 100.0f)
        Common::debug().draw(dest);

    planner[robot_num].init(OwnRobot[robot_num].state().position, dest, 90.0f);
    Common::Vec2 wayp = planner[robot_num].plan();

    planner[robot_num].draw();

    OwnRobot[robot_num].target.position = wayp;

    Common::Vec2 motion_cmd = OwnRobot[robot_num].computeMotion(velocityProfile);

    motion_cmd = dss->ComputeSafeMotion(robot_num, Common::Vec2(motion_cmd.x, motion_cmd.y));

    OwnRobot[robot_num].move(motion_cmd);
}
} // namespace Tyr::Soccer
