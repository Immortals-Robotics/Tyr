#pragma once

namespace Tyr::Soccer
{
struct VelocityProfile
{
    float max_spd;
    float max_dec;
    float max_acc;

    static constexpr VelocityProfile sooski()
    {
        return {
            .max_spd = 450.0f,
            .max_dec = 2700.0f,
            .max_acc = 1620.0f,
        };
    }

    static constexpr VelocityProfile aroom()
    {
        return {
            .max_spd = 900.0f,
            .max_dec = 2700.0f,
            .max_acc = 2160.0f,
        };
    }

    static constexpr VelocityProfile mamooli()
    {
        return {
            .max_spd = 3800.0f,
            .max_dec = 4850.5f,
            .max_acc = 3100.0f,
        };
    }

    static constexpr VelocityProfile kharaki()
    {
        return {
            .max_spd = 3800.0f,
            .max_dec = 5860.0f,
            .max_acc = 3910.0f,
        };
    }
};

} // namespace Tyr::Soccer
