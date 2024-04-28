#include "../ai.h"

namespace Tyr::Soccer
{
bool Ai::attackFuckingAngle()
{
    bool ans = false;

    static int attfuanHys = 0;

    int oppAttack = findKickerOpp(-1);
    if (oppAttack != -1)
    {
        Common::Angle ownGoalAngle = Common::Vec2(side * field_width, 0).angleWith(ball.Position);
        Common::Angle oppGoalAngle = ball.Position.angleWith(Common::Vec2(-side * field_height, 0));

        if (std::fabs((ownGoalAngle - oppGoalAngle).deg()) > 80.0f)
        {
            ans = true;
        }
    }

    if (ans)
    {
        attfuanHys = 30;
        return true;
    }

    if (attfuanHys > 0)
    {
        attfuanHys--;
        return true;
    }

    return false;
}
} // namespace Tyr::Soccer
