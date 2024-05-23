#pragma once

#include "../robot/velocity_profile.h"

namespace Tyr::Soccer
{

struct SyncPoint
{
    int robot_id = 0;
    int point_id = 0;

    SyncPoint() = default;

    SyncPoint(const Protos::Immortals::SyncPoint &t_point)
    {
        robot_id = t_point.robot_id();
        point_id = t_point.point_id();
    }
};

struct SyncData
{
    std::vector<SyncPoint> points;

    SyncData() = default;

    SyncData(const Protos::Immortals::SyncData &t_data)
    {
        for (const auto &point : t_data.point())
            points.emplace_back(point);
    }
};

struct Waypoint
{
    enum class Type
    {
        Position = 0,
        Time     = 1,
    };

    Type            type = Type::Position;
    Common::Vec2    position;
    bool            need_rrt         = true;
    float           speed            = 100.0f;
    VelocityProfile velocity_profile = VelocityProfile::mamooli();
    float           tolerance        = 100.0f;
    float           time;

    Waypoint() = default;

    Waypoint(const Protos::Immortals::Waypoint &t_waypoint)
    {
        type     = static_cast<Type>(t_waypoint.type());
        position = Common::Vec2(t_waypoint.x(), t_waypoint.y());
        need_rrt = t_waypoint.need_rrt();
        speed    = t_waypoint.speed();

        switch (t_waypoint.velocity_profile())
        {
        case Protos::Immortals::Waypoint_VelocityProfile_Aroom:
            velocity_profile = VelocityProfile::aroom();
            break;
        case Protos::Immortals::Waypoint_VelocityProfile_Mamooli:
            velocity_profile = VelocityProfile::mamooli();
            break;
        case Protos::Immortals::Waypoint_VelocityProfile_Kharaki:
            velocity_profile = VelocityProfile::kharaki();
            break;
        }

        tolerance = t_waypoint.tolerance();
        time      = t_waypoint.time();
    }
};

struct Role
{
    enum class Afterlife
    {
        Gool     = 0,
        OneTouch = 1,
        Shirje   = 2,
        Allaf    = 3,
    };

    int                   id;
    std::string           name;
    std::vector<Waypoint> path;
    Afterlife             afterlife;

    Role() = default;

    Role(const Protos::Immortals::Role &t_role)
    {
        id   = t_role.id();
        name = t_role.name();

        for (const auto &waypoint : t_role.path())
            path.emplace_back(waypoint);

        afterlife = static_cast<Afterlife>(t_role.afterlife());
    }
};

struct Strategy
{
    std::string           name;
    std::vector<Role>     roles;
    std::vector<SyncData> syncs;
    Common::Rect          area;

    Strategy() = default;

    Strategy(const Protos::Immortals::Strategy &t_strategy)
    {
        name = t_strategy.name();

        for (const auto &role : t_strategy.role())
            roles.emplace_back(role);

        for (const auto &data : t_strategy.sync())
            syncs.emplace_back(data);

        area = Common::Rect{{t_strategy.min_x(), t_strategy.min_y()}, {t_strategy.max_x(), t_strategy.max_y()}};
    }
};

struct PlayBook
{
    std::vector<Strategy> strategies;
    std::vector<float>    weights;

    PlayBook() = default;

    PlayBook(const Protos::Immortals::PlayBook &t_playbook)
    {
        for (const auto &strategy : t_playbook.strategy())
            strategies.emplace_back(strategy);

        for (const float weight : t_playbook.weight())
            weights.push_back(weight);
    }
};
} // namespace Tyr::Soccer
