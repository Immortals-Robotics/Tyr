#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::kickoff_their_one_wall()
{
    GKHi(gk, 1);
    DefMid(def, rw, lw, NULL, false);

    ERRTSetObstacles(dmf, true, true);
    OwnRobot[dmf].face(ball.Position);
    ERRTNavigate2Point(
        dmf,
        Common::point_on_connecting_line(ball.Position, Common::vec2(side * field_width, 0),
                                      Common::distance(ball.Position, Common::vec2(side * field_width, 0)) / 2.0f),
        0, 40, &VELOCITY_PROFILE_MAMOOLI);

    int indexP = -1;
    int indexN = -1;

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if ((fabs(OppRobot[i].Position.x) < 1500) && (fabs(OppRobot[i].Position.y) > 600) &&
            (OppRobot[i].seenState != Common::CompletelyOut))
        {
            if (OppRobot[i].Position.y > 0)
                indexP = i;
            if (OppRobot[i].Position.y < 0)
                indexN = i;
        }
    }

    std::cout << indexN << "	" << indexP << std::endl;

    if (indexN != -1)
    {
        if (side == -1)
        {
            ERRTSetObstacles(mid1, true, true);
            OwnRobot[mid1].face(Common::vec2(-side * field_width, 0));
            ERRTNavigate2Point(mid1, Common::point_on_connecting_line(OppRobot[indexN].Position,
                                                                   Common::vec2(side * field_width, 0),
                                                                   (fabs(OppRobot[indexN].Position.x) + 14) * 1.5));
            markMap[&mid1] = indexN;
        }
        else
        {
            ERRTSetObstacles(mid2, true, true);
            OwnRobot[mid2].face(Common::vec2(-side * field_width, 0));
            ERRTNavigate2Point(
                mid2, Common::point_on_connecting_line(OppRobot[indexN].Position, Common::vec2(side * field_width, 0),
                                                    (std::fabs(OppRobot[indexN].Position.x) + 14) * 1.5));
            markMap[&mid2] = indexN;
        }
    }
    else
    {
        if (side == -1)
        {
            ERRTSetObstacles(mid1, true, true);
            OwnRobot[mid1].face(ball.Position);
            ERRTNavigate2Point(
                mid1,
                Common::circle_around_point(ball.Position,
                                          Common::Angle::fromDeg(20.0f) +
                                              Common::angle_with(ball.Position, Common::vec2(side * field_width, 0)),
                                          790.0f),
                0, 100);
            markMap[&mid1] = -1;
        }
        else
        {
            ERRTSetObstacles(mid2, true, true);
            OwnRobot[mid2].face(ball.Position);
            ERRTNavigate2Point(
                mid2,
                Common::circle_around_point(ball.Position,
                                          Common::Angle::fromDeg(-20.0f) +
                                              Common::angle_with(ball.Position, Common::vec2(side * field_width, 0)),
                                          790.0f),
                0, 100);
            markMap[&mid2] = -1;
        }
    }

    if (indexP != -1)
    {
        if (side == 1)
        {
            ERRTSetObstacles(mid1, true, true);
            OwnRobot[mid1].face(Common::vec2(-side * field_width, 0));
            ERRTNavigate2Point(mid1, Common::point_on_connecting_line(OppRobot[indexP].Position,
                                                                   Common::vec2(side * field_width, 0),
                                                                   (fabs(OppRobot[indexP].Position.x) + 14) * 1.5));
            markMap[&mid1] = indexP;
        }
        else
        {
            ERRTSetObstacles(mid2, true, true);
            OwnRobot[mid2].face(Common::vec2(-side * field_width, 0));
            ERRTNavigate2Point(mid2, Common::point_on_connecting_line(OppRobot[indexP].Position,
                                                                   Common::vec2(side * field_width, 0),
                                                                   (fabs(OppRobot[indexP].Position.x) + 14) * 1.5));
            markMap[&mid2] = indexP;
        }
    }
    else
    {
        if (side == 1)
        {
            ERRTSetObstacles(mid1, true, true);
            OwnRobot[mid1].face(ball.Position);
            ERRTNavigate2Point(
                mid1,
                Common::circle_around_point(ball.Position,
                                          Common::Angle::fromDeg(20) +
                                              Common::angle_with(ball.Position, Common::vec2(side * field_width, 0)),
                                          790),
                0, 100);
            markMap[&mid1] = -1;
        }
        else
        {
            ERRTSetObstacles(mid2, true, true);
            OwnRobot[mid2].face(ball.Position);
            ERRTNavigate2Point(
                mid2,
                Common::circle_around_point(ball.Position,
                                          Common::Angle::fromDeg(-20) +
                                              Common::angle_with(ball.Position, Common::vec2(side * field_width, 0)),
                                          790),
                0, 100);
            markMap[&mid2] = -1;
        }
    }

    DefenceWall(attack, true);
}
} // namespace Tyr::Soccer
