#pragma once

namespace Tyr::Soccer
{
struct VelocityProfile
{
    float max_spd;
    float max_dec;
    float max_acc;

    inline static constexpr VelocityProfile sooski()
    {
        return {
            .max_spd = 10.0f,
            .max_dec = 1.0f,
            .max_acc = 0.6f,
        };
    }

    inline static constexpr VelocityProfile aroom()
    {
        return {
            .max_spd = 20.0f,
            .max_dec = 1.0f,
            .max_acc = 0.8f,
        };
    }

    inline static constexpr VelocityProfile mamooli()
    {
        return {
            .max_spd = 40.0f,
            .max_dec = 1.5f,
            .max_acc = 1.0f,
        };
    }

    inline static constexpr VelocityProfile kharaki()
    {
        return {
            .max_spd = 40.0f,
            .max_dec = 1.8f,
            .max_acc = 1.3f,
        };
    }
};

} // namespace Tyr::Soccer
