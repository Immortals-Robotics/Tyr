#pragma once

namespace Tyr::Soccer
{
struct VelocityProfile
{
    Common::Vec2 max_spd;
    Common::Vec2 max_dec;
    Common::Vec2 max_acc;
    float max_w_acc;
    float max_w_dec;
};
} // namespace Tyr::Soccer
