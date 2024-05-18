#include "Robot.h"

Robot::Robot()
{
    finalRole = NormalPlay;
    waypointCounter=0;    
    for(int i=0;i<60;i++)
        waypoint[i] = new WayPoint();

}
