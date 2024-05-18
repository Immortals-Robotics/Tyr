#include "waypoint.h"

WayPoint::WayPoint()
{
    wayPointType = 0;
    needRRT=true;
    speed = 100.0;
    velProfile = MAMOOLIE;
    tolerance = 100.0;
    time = 8;
}
