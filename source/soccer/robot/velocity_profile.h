#pragma once

namespace Tyr::Soccer
{
struct VelocityProfile
{
    float speed = 0.0f;
    float acceleration = 0.0f;

    VelocityProfile() = default;

    explicit VelocityProfile(const Common::Config::VelocityProfile &config)
        : speed(config.speed), acceleration(config.acceleration)
    {}

    static VelocityProfile sooski()
    {
        return VelocityProfile{Common::config().soccer.velocity_profile_sooski};
    }

    static VelocityProfile aroom()
    {
        return VelocityProfile{Common::config().soccer.velocity_profile_aroom};
    }

    static VelocityProfile mamooli()
    {
        return VelocityProfile{Common::config().soccer.velocity_profile_mamooli};
    }

    static VelocityProfile kharaki()
    {
        return VelocityProfile{Common::config().soccer.velocity_profile_kharaki};
    }
};

} // namespace Tyr::Soccer
