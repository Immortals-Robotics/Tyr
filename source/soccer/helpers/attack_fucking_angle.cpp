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
        Common::Angle ownGoalAngle = ownGoal().angleWith(m_world_state.ball.position);
        Common::Angle oppGoalAngle = m_world_state.ball.position.angleWith(oppGoal());

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
