#pragma once

#include "skill.h"

namespace Tyr::Soccer::Skill
{

class Navigation final : public ISkill
{
public:
    struct Params
    {
        Common::Vec2     dest;
        int              speed;
        VelocityProfile *velocityProfile;
        bool             use_dss;
    };

    void setParams(const Params &t_params);

    void execute(Robot *t_robot) override;
};

} // namespace Tyr::Soccer::Skill
