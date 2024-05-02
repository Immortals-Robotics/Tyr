#include "../ai.h"

namespace Tyr::Soccer
{
bool Ai::attackFuckingAngle()
{
    bool ans = false;

    static int hys = 0;

    int oppAttack = findKickerOpp(-1);
    if (oppAttack != -1)
    {
        Common::Angle ownGoalAngle = Common::Vec2(side * field_width, 0).angleWith(ball.position);
        Common::Angle oppGoalAngle = ball.position.angleWith(Common::Vec2(-side * field_height, 0));

        if (std::fabs((ownGoalAngle - oppGoalAngle).deg()) > 80.0f)
        {
            ans = true;
        }
    }

    if (ans)
    {
        hys = 30;
        return true;
    }

    if (hys > 0)
    {
        hys--;
        return true;
    }

    return false;
}
} // namespace Tyr::Soccer
