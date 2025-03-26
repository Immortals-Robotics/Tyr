#include "open_angle.h"

#include "../globals/field.h"
#include "../robot/robot.h"

namespace Tyr::Soccer
{
// TODO: use Common::Angle instead
float normalizeAngleR(float angle) // radian
{
    while (angle > 3.1415f)
        angle -= 2 * 3.1415f;
    while (angle < -3.1415f)
        angle += 2 * 3.1415f;

    return (angle);
}

OpenAngle OpenAngle::calculateOpenAngleToGoal(const Common::Vec2 t_pos, const Robot& t_robot)
{
    int  counter = 0;
    bool oops    = 0;

    std::vector<Common::Vec2> obstacles;
    obstacles.reserve(50);

    const Common::Vec2 t2      = Field::oppGoalPostTop();
    const Common::Vec2 t1      = Field::oppGoalPostBottom();
    const Common::Vec2 MidGoal = Field::oppGoal();

    float midGoalAngel = atan2((MidGoal.y - t_pos.y), (MidGoal.x - t_pos.x));
    float t1Angel      = atan2((t1.y - t_pos.y), (t1.x - t_pos.x));
    float t2Angel      = atan2((t2.y - t_pos.y), (t2.x - t_pos.x));

    if (std::fabs(t1Angel - t2Angel) > 3.1415f)
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

    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        if (State::world().own_robot[i].seen_state != Common::SeenState::CompletelyOut && (i != t_robot.state().vision_id))
        {
            obstacles.emplace_back(State::world().own_robot[i].position);
        }
    }
    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        if (State::world().opp_robot[i].seen_state != Common::SeenState::CompletelyOut)
        {
            obstacles.emplace_back(State::world().opp_robot[i].position);
        }
    }

    ///////////////////////////////////////////////////////////////

    for (int i = 0; i < obstacles.size(); i++)
    {

        d[counter][0] = atan2(obstacles[i].y - t_pos.y, obstacles[i].x - t_pos.x);

        float dd = std::atan(90.0f / t_pos.distanceTo(obstacles[i]));
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
                dtmp[0] += 2 * 3.1415f;
            if (dtmp[1] < 0)
                dtmp[1] += 2 * 3.1415f;
            if (ttmp[0] < 0)
                ttmp[0] += 2 * 3.1415f;
            if (ttmp[1] < 0)
                ttmp[1] += 2 * 3.1415f;
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
        step += 2 * 3.1415f / 100.0f;
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
                    dtmp[0] += 2 * 3.1415f;
                if (dtmp[1] < 0)
                    dtmp[1] += 2 * 3.1415f;
                if (ang < 0)
                    ang += 2 * 3.1415f;
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

    OpenAngle finalAns;

    if (maxFree == 0)
        finalAns = OpenAngle{Common::Angle::fromRad(midGoalAngel), Common::Angle::fromRad(0.0f)};
    else
        finalAns = OpenAngle{Common::Angle::fromRad(max), Common::Angle::fromRad(maxFree * step)};

#if 1
    // TODO: wtf :/
    static Common::MedianFilter<Common::Angle> freeAngleFilter[Common::Config::Common::kMaxRobots];

    freeAngleFilter[t_robot.state().vision_id].add(finalAns.center);
    finalAns.center = freeAngleFilter[t_robot.state().vision_id].current();

#endif

    return finalAns;
}
} // namespace Tyr::Soccer
