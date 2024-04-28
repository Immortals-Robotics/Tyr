#pragma once

namespace Tyr::Soccer
{
class TrapezoidPlanner
{
private:
    Common::Vec2 max_acc;
    Common::Vec2 max_dec;
    Common::Vec2 max_spd;

    float period;

    Common::RobotState *init_state;
    Common::RobotState *final_state;

    Common::Vec3 oldAns;

    float Plan1D(float d, float v, float tmp_max_spd, float tmp_max_acc, float tmp_max_dec);

public:
    TrapezoidPlanner();
    TrapezoidPlanner(Common::Vec2 _max_acc, Common::Vec2 _max_dec, Common::Vec2 _max_spd, float _period);

    void init(Common::Vec2 _max_acc, Common::Vec2 _max_dec, Common::Vec2 _max_spd, float _period);

    Common::Vec3 Plan(Common::RobotState *_init, Common::RobotState *_final);
};
} // namespace Tyr::Soccer
