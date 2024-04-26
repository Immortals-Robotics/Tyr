#pragma once

namespace Tyr::Soccer
{
struct VelocityProfile
{
    Common::vec2 max_spd;
    Common::vec2 max_dec;
    Common::vec2 max_acc;
    float max_w_acc;
    float max_w_dec;
};
} // namespace Tyr::Soccer
