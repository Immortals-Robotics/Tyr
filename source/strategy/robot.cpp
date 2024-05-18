#include "Robot.h"

namespace Tyr::Strategy
{
Robot::Robot()
{
    for (int i = 0; i < 60; i++)
        waypoint[i] = new WayPoint();
}
} // namespace Tyr::Strategy
