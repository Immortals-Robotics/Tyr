#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::Navigate2Point(int robot_num, Common::Vec2 dest, float speed, VelocityProfile::Type velocityProfile,
                        bool use_dss)
{
    if (m_ref_state.shouldSlowDown() &&
        (velocityProfile == VelocityProfile::Type::Mamooli || velocityProfile == VelocityProfile::Type::Kharaki ||
         velocityProfile == VelocityProfile::Type::Killer))
    {
        velocityProfile = VelocityProfile::Type::Aroom;
    }

    if (navigated[robot_num] && OwnRobot[robot_num].state().seen_state != Common::SeenState::CompletelyOut)
    {
        Common::logWarning("Robot {} is navigated more than once", OwnRobot[robot_num].vision_id);
    }

    OwnRobot[robot_num].target.position.x = dest.x;
    OwnRobot[robot_num].target.position.y = dest.y;

    Common::Vec3 motion_cmd = OwnRobot[robot_num].ComputeMotionCommand(speed, velocityProfile);

    if (use_dss)
    {
        const Common::Vec2 safe_motion_cmd =
            dss->ComputeSafeMotion(robot_num, Common::Vec2(motion_cmd.x, motion_cmd.y));
        motion_cmd.x = safe_motion_cmd.x;
        motion_cmd.y = safe_motion_cmd.y;
    }
    OwnRobot[robot_num].MoveByMotion(motion_cmd);

    navigated[robot_num] = true;
}

void Ai::ERRTNavigate2Point(int robot_num, Common::Vec2 dest, float speed, const VelocityProfile::Type velocityProfile)
{
    if (OwnRobot[robot_num].state().seen_state == Common::SeenState::CompletelyOut)
    {
        Halt(robot_num);
    }
    else
    {
        if (OwnRobot[robot_num].state().position.distanceTo(dest) > 100.0f)
            Common::debug().draw(dest);

        planner[robot_num].init(OwnRobot[robot_num].state().position, dest, 90.0f);
        Common::Vec2 wayp = planner[robot_num].plan();

        planner[robot_num].draw();

        Navigate2Point(robot_num, wayp, speed, velocityProfile, true);
    }
}
} // namespace Tyr::Soccer
