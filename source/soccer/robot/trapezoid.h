#pragma once

namespace Tyr::Soccer
{
class TrapezoidPlanner
{
private:
    Common::vec2 max_acc;
    Common::vec2 max_dec;
    Common::vec2 max_spd;

    float period;

    Common::RobotState *init_state;
    Common::RobotState *final_state;

    Common::vec3 oldAns;

    float Plan1D(float d, float v, float tmp_max_spd, float tmp_max_acc, float tmp_max_dec);

public:
    TrapezoidPlanner();
    TrapezoidPlanner(Common::vec2 _max_acc, Common::vec2 _max_dec, Common::vec2 _max_spd, float _period);

    void init(Common::vec2 _max_acc, Common::vec2 _max_dec, Common::vec2 _max_spd, float _period);

    Common::vec3 Plan(Common::RobotState *_init, Common::RobotState *_final);
};
} // namespace Tyr::Soccer
