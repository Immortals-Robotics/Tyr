#pragma once

#include "tactic.h"

namespace Tyr::Soccer
{
class DefTactic final : public Tactic
{
public:
    explicit DefTactic(const int t_num) : m_num(t_num)
    {}

    static const Id kId;
    Id              id() const override
    {
        return kId;
    }

    void execute(Robot &t_robot) override;

private:
    int m_num = 0;

    int m_hys_dive = 0;
    static inline int s_hys_select = 0;

    Common::Vec2 defStatic_1();
    Common::Vec2 defStatic_2();
    void defShirje_1(Robot& t_robot_1);
    void defShirje_2(Robot& t_robot_2);
};
} // namespace Tyr::Soccer
