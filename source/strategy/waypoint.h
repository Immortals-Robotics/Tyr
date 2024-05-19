#pragma once

namespace Tyr::Strategy
{
class WayPoint : public QLabel
{
public:
    WayPoint() = default;

    int          waypoint_type = 0;
    QVector<int> sync_waypoints;
    bool         need_rrt         = true;
    float        speed            = 100.0f;
    int          velocity_profile = Protos::Immortals::Waypoint_VelocityProfile_Mamooli;
    float        tolerance        = 100.0f;
    float        time             = 8;
};
} // namespace Tyr::Strategy
