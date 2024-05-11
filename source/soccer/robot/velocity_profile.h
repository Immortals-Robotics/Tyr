#pragma once

namespace Tyr::Soccer
{
struct VelocityProfile
{
    enum class Type
    {
        BallPlaceKheyliSooski,
        Aroom,
        Kharaki,
        Mamooli,
        Killer,
    };

    Common::Vec2 max_spd;
    Common::Vec2 max_dec;
    Common::Vec2 max_acc;
    float        max_w_acc;
    float        max_w_dec;

    constexpr VelocityProfile(const VelocityProfile::Type t_type)
    {
        switch (t_type)
        {
        case VelocityProfile::Type::BallPlaceKheyliSooski:
            max_spd   = Common::Vec2(10.0f);
            max_dec   = Common::Vec2(1.0f);
            max_acc   = Common::Vec2(0.6f);
            max_w_acc = 40.0f;
            max_w_dec = 140.0f;
            break;

        case VelocityProfile::Type::Aroom:
            max_spd   = Common::Vec2(20.0f);
            max_dec   = Common::Vec2(1.0f);
            max_acc   = Common::Vec2(0.8f);
            max_w_acc = 40.0f;
            max_w_dec = 140.0f;
            break;

        case VelocityProfile::Type::Mamooli:
            max_spd   = Common::Vec2(40.0f);
            max_dec   = Common::Vec2(1.5f);
            max_acc   = Common::Vec2(1.0f);
            max_w_acc = 40.0f;
            max_w_dec = 140.0f;
            break;

        case VelocityProfile::Type::Kharaki:
            max_spd   = Common::Vec2(40.0f);
            max_dec   = Common::Vec2(1.8f);
            max_acc   = Common::Vec2(1.3f);
            max_w_acc = 40.0f;
            max_w_dec = 140.0f;
            break;

        case VelocityProfile::Type::Killer:
            max_spd   = Common::Vec2(40.0f);
            max_dec   = Common::Vec2(1.8f);
            max_acc   = Common::Vec2(1.3f);
            max_w_acc = 40.0f;
            max_w_dec = 140.0f;
            break;

        default:
            Common::logWarning("Unknown velocity profile type: {}", (int) t_type);
            break;
        }
    }
};

} // namespace Tyr::Soccer
