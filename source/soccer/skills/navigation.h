#pragma once

#include "../dss/dss.h"
#include "../errt/errt.h"
#include "../obstacle/obstacle.h"

namespace Tyr::Soccer::Skill
{

class Navigation
{
public:
    struct Params
    {
        Common::Vec2 dest;

        int             speed = 80;
        VelocityProfile velocity_profile;

        bool use_errt = true;
        bool use_dss  = true;

        bool obs_ball  = false;
        bool obs_field = true;
    };

    void execute(Robot *t_robot, const Params &t_params);

private:
    void setObstacles(Robot *t_robot, const Params &t_params);

    ObsMap m_obs_map;

    Planner m_planner;
    Dss     m_dss;

    bool m_navigated;
};

} // namespace Tyr::Soccer::Skill
