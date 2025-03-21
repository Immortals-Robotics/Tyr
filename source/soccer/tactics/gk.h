#pragma once

#include "tactic.h"

namespace Tyr::Soccer
{
class GkTactic final : public Tactic
{
public:
    GkTactic() = default;

    static const Id kId;
    Id id() const override
    {
        return kId;
    }

    void execute(Robot &t_robot) override;

private:
    // TODO: make these non-static once we persist tactics
    static inline bool m_intercepting = false;
    static inline int m_hys = 0;

    // TODO: this should probably be an skill
    void shirje(Robot& t_robot);

    NavigationFlags getNavigationFlags() const;
};
} // namespace Tyr::Soccer
