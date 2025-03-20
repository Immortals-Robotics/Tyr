#pragma once

namespace Tyr::Soccer
{
class Robot;
class Skill
{
public:
    virtual ~Skill() = default;

    using Id = const void*;
    virtual Id id() const = 0;

    virtual void execute(Robot& t_robot) = 0;
};
} // namespace Tyr::Soccer
