#pragma once

namespace Tyr::Soccer
{
class Robot;

// TODO: add cost calculation function
class Tactic
{
public:
    virtual ~Tactic() = default;

    using Id = const void*;
    virtual Id id() const = 0;

    virtual void execute(Robot& t_robot) = 0;
};
} // namespace Tyr::Soccer
