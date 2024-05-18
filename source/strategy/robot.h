#pragma once

#include "waypoint.h"

#define NormalPlay 0
#define OneTouch 1
#define Shirje 2
#define Allaf 3

namespace Tyr::Strategy
{
class Robot
{
public:
    Robot();
    int       ID;
    QString   Name;
    int       finalRole;
    WayPoint *waypoint[60];
    int       waypointCounter;
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
