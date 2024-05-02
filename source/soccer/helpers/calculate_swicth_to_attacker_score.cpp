#include "../ai.h"

namespace Tyr::Soccer
{
float Ai::calculateSwicthToAttackerScore(int robot_num)
{
    if (OwnRobot[robot_num].State.seen_state == Common::SeenState::CompletelyOut)
        return -1;

    if (robot_num != mid1 && robot_num != mid2)
        return -1;

    if (!isDefending && oneTouchDetector[robot_num].IsArriving(45, 150))
        return -1;

    float currAttBallDis = OwnRobot[attack].State.position.distanceTo(ball.position);

    if (OwnRobot[attack].State.seen_state == Common::SeenState::CompletelyOut)
        currAttBallDis = 20000;

    int marked_id = -1;
    for (auto it = markMap.begin(); it != markMap.end(); ++it)
    {
        if (*it->first == robot_num)
        {
            marked_id = it->second;
            break;
        }
    }

    if (isDefending && marked_id != -1)
    {
        int opp = marked_id;
        if ((OppRobot[opp].position.distanceTo(ball.position) < 400) &&
            (OwnRobot[robot_num].State.position.distanceTo(ball.position) < 400) && (currAttBallDis > 600) &&
            (ball.velocity.length() < 500))
        {
            return 0;
        }
        else
            return -1;
    }

    float disToBall = OwnRobot[robot_num].State.position.distanceTo(ball.position);
    if (disToBall > currAttBallDis - 500)
        return 0;

    float dis_score = (currAttBallDis - disToBall - 500) / 1000.0f;
    dis_score       = std::min(1.0f, std::max(0.0f, dis_score));

    return dis_score;
}
} // namespace Tyr::Soccer
