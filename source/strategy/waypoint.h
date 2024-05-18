#pragma once

#define AROOME 0
#define MAMOOLIE 1
#define KHARAKIE 2

namespace Tyr::Strategy
{
class WayPoint : public QLabel
{
public:
    WayPoint();
    int          wayPointType;
    QVector<int> syncWayPoints;
    bool         needRRT;
    float        speed;
    int          velProfile;
    float        tolerance;
    float        time;
};
} // namespace Tyr::Strategy
