#pragma once

#include "waypoint.h"

namespace Tyr::Strategy
{
class Robot
{
public:
    Robot();
    QString   Name;
    int       finalRole = 0;
    WayPoint *waypoint[60];
    int       waypointCounter = 0;
    enum RobotType
    {
        GK,
        def1,
        def2,
        MidFielder,
        Passgir,
        Attack
    };
};
} // namespace Tyr::Strategy
