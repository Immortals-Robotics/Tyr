#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::WaitForGool(int robot_num, bool chip)
{
    Common::Vec2 pos =
        CalculatePassPos(robot_num, Common::Vec2(-side * 3025, 0), OwnRobot[robot_num].State.Position, -1600);

    OwnRobot[robot_num].face(Common::Vec2(-side * field_width, 0));

    ERRTSetObstacles(robot_num, 0, 1);
    ERRTNavigate2Point(robot_num, pos, 0, 100, &VELOCITY_PROFILE_MAMOOLI);

    OwnRobot[robot_num].Shoot(100);
    OwnRobot[robot_num].Dribble(15);
}
} // namespace Tyr::Soccer
