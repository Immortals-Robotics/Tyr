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
            .max_spd = 450.0f,
            .max_dec = 45.0f,
            .max_acc = 27.0f,
        };
    }

    inline static constexpr VelocityProfile aroom()
    {
        return {
            .max_spd = 900.0f,
            .max_dec = 45.0f,
            .max_acc = 36.0f,
        };
    }

    inline static constexpr VelocityProfile mamooli()
    {
        return {
            .max_spd = 1800.0f,
            .max_dec = 67.5f,
            .max_acc = 45.0f,
        };
    }

    inline static constexpr VelocityProfile kharaki()
    {
        return {
            .max_spd = 1800.0f,
            .max_dec = 81.0f,
            .max_acc = 58.5f,
        };
    }
};

} // namespace Tyr::Soccer
