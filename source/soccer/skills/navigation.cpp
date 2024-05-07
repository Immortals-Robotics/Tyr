#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::Navigate2Point(int robot_num, Common::Vec2 dest, int speed, VelocityProfile *velocityProfile, bool use_dss)
{
    OwnRobot[robot_num].target.position.x = dest.x;
    OwnRobot[robot_num].target.position.y = dest.y;

    if (velocityProfile == nullptr)
        velocityProfile = &this->VELOCITY_PROFILE_MAMOOLI;

    Common::Vec3 motion_cmd = OwnRobot[robot_num].ComputeMotionCommand(speed, *velocityProfile);

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

void Ai::ERRTNavigate2Point(int robot_num, Common::Vec2 dest, int speed, VelocityProfile *velocityProfile)
{
    // Navigate2Point(robot_num, dest,accurate,speed,velocityProfile);
    // return;
    if (OwnRobot[robot_num].state().seen_state == Common::SeenState::CompletelyOut)
        Halt(robot_num);
    else
    {
        planner[robot_num].init(OwnRobot[robot_num].state().position, dest, 90.0f);
        Common::Vec2 wayp = planner[robot_num].plan();

        planner[robot_num].draw();

        // if ( planner[robot_num].getWayPointNum ( ) <= 2 )
        Navigate2Point(robot_num, wayp, speed, velocityProfile, true);
        // else
        //	Navigate2Point ( robot_num , wayp , false , speed , velocityProfile );
        // Navigate2Point ( robot_num , dest , accurate , speed );
    }
}
} // namespace Tyr::Soccer
