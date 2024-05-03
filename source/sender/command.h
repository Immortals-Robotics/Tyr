#pragma once

namespace Tyr::Sender
{
struct Command
{
    int vision_id = -1;

    bool halted = true;

    Common::Vec3 motion;

    Common::Angle current_angle;
    Common::Angle target_angle;

    int shoot    = 0;
    int chip     = 0;
    int dribbler = 0;
};
} // namespace Tyr::Sender