#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::kickoff_their_one_wall()
{
    GKHi(gk, 1);
    DefMid(def, rw, lw, nullptr, false);

    ERRTSetObstacles(dmf, true, true);
    OwnRobot[dmf].face(Common::worldState().ball.position);
    ERRTNavigate2Point(
        dmf,
        Common::worldState().ball.position.pointOnConnectingLine(
            Common::Vec2(side * Common::worldState().field.width, 0),
            Common::worldState().ball.position.distanceTo(Common::Vec2(side * Common::worldState().field.width, 0)) /
                2.0f),
        40, &VELOCITY_PROFILE_MAMOOLI);

    int indexP = -1;
    int indexN = -1;

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if ((std::fabs(Common::worldState().opp_robot[i].position.x) < 1500) &&
            (std::fabs(Common::worldState().opp_robot[i].position.y) > 600) &&
            (Common::worldState().opp_robot[i].seen_state != Common::SeenState::CompletelyOut))
        {
            if (Common::worldState().opp_robot[i].position.y > 0)
                indexP = i;
            if (Common::worldState().opp_robot[i].position.y < 0)
                indexN = i;
        }
    }

    Common::logDebug("{}    {}", indexN, indexP);

    if (indexN != -1)
    {
        if (side == -1)
        {
            ERRTSetObstacles(mid1, true, true);
            OwnRobot[mid1].face(Common::Vec2(-side * Common::worldState().field.width, 0));
            ERRTNavigate2Point(mid1, Common::worldState().opp_robot[indexN].position.pointOnConnectingLine(
                                         Common::Vec2(side * Common::worldState().field.width, 0),
                                         (std::fabs(Common::worldState().opp_robot[indexN].position.x) + 14) * 1.5));
            markMap[&mid1] = indexN;
        }
        else
        {
            ERRTSetObstacles(mid2, true, true);
            OwnRobot[mid2].face(Common::Vec2(-side * Common::worldState().field.width, 0));
            ERRTNavigate2Point(mid2, Common::worldState().opp_robot[indexN].position.pointOnConnectingLine(
                                         Common::Vec2(side * Common::worldState().field.width, 0),
                                         (std::fabs(Common::worldState().opp_robot[indexN].position.x) + 14) * 1.5));
            markMap[&mid2] = indexN;
        }
    }
    else
    {
        if (side == -1)
        {
            ERRTSetObstacles(mid1, true, true);
            OwnRobot[mid1].face(Common::worldState().ball.position);
            ERRTNavigate2Point(
                mid1,
                Common::worldState().ball.position.circleAroundPoint(
                    Common::Angle::fromDeg(20.0f) + Common::worldState().ball.position.angleWith(
                                                        Common::Vec2(side * Common::worldState().field.width, 0)),
                    790.0f),
                100);
            markMap[&mid1] = -1;
        }
        else
        {
            ERRTSetObstacles(mid2, true, true);
            OwnRobot[mid2].face(Common::worldState().ball.position);
            ERRTNavigate2Point(
                mid2,
                Common::worldState().ball.position.circleAroundPoint(
                    Common::Angle::fromDeg(-20.0f) + Common::worldState().ball.position.angleWith(
                                                         Common::Vec2(side * Common::worldState().field.width, 0)),
                    790.0f),
                100);
            markMap[&mid2] = -1;
        }
    }

    if (indexP != -1)
    {
        if (side == 1)
        {
            ERRTSetObstacles(mid1, true, true);
            OwnRobot[mid1].face(Common::Vec2(-side * Common::worldState().field.width, 0));
            ERRTNavigate2Point(mid1, Common::worldState().opp_robot[indexP].position.pointOnConnectingLine(
                                         Common::Vec2(side * Common::worldState().field.width, 0),
                                         (std::fabs(Common::worldState().opp_robot[indexP].position.x) + 14) * 1.5));
            markMap[&mid1] = indexP;
        }
        else
        {
            ERRTSetObstacles(mid2, true, true);
            OwnRobot[mid2].face(Common::Vec2(-side * Common::worldState().field.width, 0));
            ERRTNavigate2Point(mid2, Common::worldState().opp_robot[indexP].position.pointOnConnectingLine(
                                         Common::Vec2(side * Common::worldState().field.width, 0),
                                         (std::fabs(Common::worldState().opp_robot[indexP].position.x) + 14) * 1.5));
            markMap[&mid2] = indexP;
        }
    }
    else
    {
        if (side == 1)
        {
            ERRTSetObstacles(mid1, true, true);
            OwnRobot[mid1].face(Common::worldState().ball.position);
            ERRTNavigate2Point(
                mid1,
                Common::worldState().ball.position.circleAroundPoint(
                    Common::Angle::fromDeg(20) + Common::worldState().ball.position.angleWith(
                                                     Common::Vec2(side * Common::worldState().field.width, 0)),
                    790),
                100);
            markMap[&mid1] = -1;
        }
        else
        {
            ERRTSetObstacles(mid2, true, true);
            OwnRobot[mid2].face(Common::worldState().ball.position);
            ERRTNavigate2Point(
                mid2,
                Common::worldState().ball.position.circleAroundPoint(
                    Common::Angle::fromDeg(-20) + Common::worldState().ball.position.angleWith(
                                                      Common::Vec2(side * Common::worldState().field.width, 0)),
                    790),
                100);
            markMap[&mid2] = -1;
        }
    }

    DefenceWall(attack, true);
}
} // namespace Tyr::Soccer
