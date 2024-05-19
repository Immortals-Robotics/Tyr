#pragma once

#include "waypoint.h"

namespace Tyr::Strategy
{
class Robot
{
public:
    enum Type
    {
        GK = 0,
        Def,
        Dmf,
        Lmf,
        Rmf,
        Attack,
        Rw,
        Lw,
    };

    Robot();

    QString   name;
    int       final_role = 0;
    WayPoint *waypoint[60];
    int       waypoint_counter = 0;
};
} // namespace Tyr::Strategy
