#include "../ai.h"

namespace Tyr::Soccer
{
float normalizeAngleR(float angle) // radian
{
    while (angle > 3.1415)
        angle -= 2 * 3.1415;
    while (angle < -3.1415)
        angle += 2 * 3.1415;

    return (angle);
}

Ai::OpenAngle Ai::calculateOpenAngleToGoal(Common::Vec2 p1, int robot_num)
{

    int          counter  = 0;
    bool         oops     = 0;
    int          obsCount = 0;
    int          obs[50][2];
    Common::Vec2 t2      = Common::Vec2(-Common::worldState().field.width * side, Common::worldState().field.goal_width / 2.0);
    Common::Vec2 t1      = Common::Vec2(-Common::worldState().field.width * side, -Common::worldState().field.goal_width / 2.0);
    Common::Vec2 MidGoal = Common::Vec2(-Common::worldState().field.width * side, 0);
    float        midGoalAngel;
    float        t1Angel;
    float        t2Angel;
    midGoalAngel = atan2((MidGoal.y - p1.y), (MidGoal.x - p1.x));
    t1Angel      = atan2((t1.y - p1.y), (t1.x - p1.x));
    t2Angel      = atan2((t2.y - p1.y), (t2.x - p1.x));

    if (std::fabs(t1Angel - t2Angel) > 3.1415)
        oops = 1;

    if (t1Angel > t2Angel)
    {
        float tmp = t1Angel;
        t1Angel   = t2Angel;
        t2Angel   = tmp;
    }
    if (oops)
    {
        float tmp = t1Angel;
        t1Angel   = t2Angel;
        t2Angel   = tmp;
    }

    float d[100][2];
    float dtmp[2], ttmp[2];

    // Set Obstacles //////////////////////////////////////////////

    for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
    {
        if ((OwnRobot[i].state().seen_state != Common::SeenState::CompletelyOut) && (i != robot_num))
        {
            obs[obsCount][0] = OwnRobot[i].state().position.x;
            obs[obsCount][1] = OwnRobot[i].state().position.y;
            obsCount++;
        }
    }
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (Common::worldState().opp_robot[i].seen_state != Common::SeenState::CompletelyOut)
        {
            obs[obsCount][0] = Common::worldState().opp_robot[i].position.x;
            obs[obsCount][1] = Common::worldState().opp_robot[i].position.y;
            obsCount++;
        }
    }

    ///////////////////////////////////////////////////////////////

    for (int i = 0; i < obsCount; i++)
    {

        d[counter][0] = atan2((obs[i][1] - p1.y), obs[i][0] - p1.x);

        float dd = std::atan(90.0f / p1.distanceTo(Common::Vec2(obs[i][0], obs[i][1])));
        if (dd < 0)
            dd = -dd;
        d[counter][1] = d[counter][0] + dd;
        d[counter][0] = d[counter][0] - dd;

        d[counter][0] = normalizeAngleR(d[counter][0]);
        d[counter][1] = normalizeAngleR(d[counter][1]);

        dtmp[0] = d[counter][0];
        dtmp[1] = d[counter][1];
        ttmp[0] = t1Angel;
        ttmp[1] = t2Angel;

        if (oops)
        {
            if (dtmp[0] < 0)
                dtmp[0] += 2 * 3.1415;
            if (dtmp[1] < 0)
                dtmp[1] += 2 * 3.1415;
            if (ttmp[0] < 0)
                ttmp[0] += 2 * 3.1415;
            if (ttmp[1] < 0)
                ttmp[1] += 2 * 3.1415;
        }

        // No Free Angel
        if (dtmp[1] > ttmp[1] && dtmp[0] < ttmp[0])
            return {Common::Angle::fromRad(midGoalAngel), Common::Angle::fromRad(0.0f)};

        if (dtmp[0] > ttmp[1] || dtmp[1] < ttmp[0])
        {
            d[counter][0] = 0;
            d[counter][1] = 0;
            continue;
        }

        if (dtmp[0] < ttmp[0])
            d[counter][0] = t1Angel;

        if (dtmp[1] > ttmp[1])
            d[counter][1] = t2Angel;

        counter++;
    }

    // Completely Free
    if (counter == 0)
    {
        return {Common::Angle::fromRad(midGoalAngel), Common::Angle::fromRad(std::fabs(t2Angel - t1Angel))};
    }

    float step = (t2Angel - t1Angel) / 100.0f;
    if (oops)
    {
        step += 2 * 3.1415 / 100.0;
    }
    bool freeAngel[100];
    for (int i = 0; i < 100; i++)
    {
        float ang = (t1Angel + i * step);
        for (int j = 0; j < counter; j++)
        {
            dtmp[0] = d[j][0];
            dtmp[1] = d[j][1];
            if (oops)
            {
                if (dtmp[0] < 0)
                    dtmp[0] += 2 * 3.1415;
                if (dtmp[1] < 0)
                    dtmp[1] += 2 * 3.1415;
                if (ang < 0)
                    ang += 2 * 3.1415;
            }

            if (ang >= dtmp[0] && ang < dtmp[1])
            {
                freeAngel[i] = 1;
                // i=(d[j][1]-t1Angel)/step;
                break;
            }
            else
                freeAngel[i] = 0;
        }
    }
    int   maxFree   = 0;
    int   freeCount = 0;
    float max       = 0;
    for (int i = 0; i < 100; i++)
    {
        if (freeAngel[i] == 1)
        {
            if (freeCount > maxFree)
            {
                maxFree = freeCount;
                max     = i - maxFree / 2;
            }
            freeCount = 0;
        }
        else
        {
            freeCount++;
        }
    }

    if (freeCount > maxFree)
    {
        maxFree = freeCount;
        max     = 100 - maxFree / 2;
        ;
    }
    max = t1Angel + max * step;
    max = normalizeAngleR(max);

    // std::cout << "	Salam Olaghe aziz :	" << maxFree << std::endl;

    OpenAngle finalAns;

    if (maxFree == 0)
        finalAns = OpenAngle{Common::Angle::fromRad(midGoalAngel), Common::Angle::fromRad(0.0f)};
    else
        finalAns = OpenAngle{Common::Angle::fromRad(max), Common::Angle::fromRad(maxFree * step)};

#if 1
    static Common::MedianFilter<Common::Angle> freeAngleFilter[Common::Setting::kMaxOnFieldTeamRobots];

    freeAngleFilter[robot_num].AddData(finalAns.center);
    finalAns.center = freeAngleFilter[robot_num].GetCurrent();

#endif

    return finalAns;
}
} // namespace Tyr::Soccer
