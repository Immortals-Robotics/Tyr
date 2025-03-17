#pragma once

namespace Immortals::Common::Soccer
{
enum class Role
{
    Unknown = 0,
    Gk,
    Def1,
    Def2,
    Mid1,
    Mid2,
    Mid3,
    Mid4,
    Mid5,
    Mid6,
    Mid7,
    Attack,
};

enum class OneTouchType
{
    OneTouch = 0,
    Shirje   = 1,
    Gool     = 2,
    Allaf    = 3,
};

class Robot
{
public:
    Robot() = default;

    explicit Robot(const Protos::Immortals::Soccer::Robot &t_robot)
    {
        id = t_robot.id();

        role = static_cast<Role>(t_robot.role());

        shoot     = t_robot.shoot();
        chip      = t_robot.chip();
        dribbler  = t_robot.dribbler();
        navigated = t_robot.navigated();
        halted    = t_robot.halted();

        one_touch_type      = static_cast<OneTouchType>(t_robot.one_touch_type());
        one_touch_arriving  = t_robot.one_touch_arriving();
        one_touch_type_used = t_robot.one_touch_type_used();

        mark_target = t_robot.mark_target();
    }

    void fillProto(Protos::Immortals::Soccer::Robot *const t_robot) const
    {
        t_robot->set_id(id);

        t_robot->set_role(static_cast<Protos::Immortals::Soccer::Role>(role));

        t_robot->set_shoot(shoot);
        t_robot->set_chip(chip);
        t_robot->set_dribbler(dribbler);
        t_robot->set_navigated(navigated);
        t_robot->set_halted(halted);

        t_robot->set_one_touch_type(static_cast<Protos::Immortals::Soccer::OneTouchType>(one_touch_type));
        t_robot->set_one_touch_arriving(one_touch_arriving);
        t_robot->set_one_touch_type_used(one_touch_type_used);

        t_robot->set_mark_target(mark_target);
    }

    int id = -1;

    Role role = Role::Unknown;

    float shoot     = 0.0f;
    float chip      = 0.0f;
    float dribbler  = 0.0f;
    bool  navigated = false;
    bool  halted    = false;

    OneTouchType one_touch_type      = OneTouchType::OneTouch;
    bool         one_touch_arriving  = false;
    bool         one_touch_type_used = false;

    int mark_target = -1;
};
} // namespace Immortals::Common::Soccer

#if FEATURE_LOGGING
template <>
struct fmt::formatter<Immortals::Common::Soccer::Role> : fmt::formatter<std::string>
{
    auto format(const Immortals::Common::Soccer::Role t_role, format_context &t_ctx) const
    {
        const char *type_str = "Unknown";
        switch (t_role)
        {
        case Immortals::Common::Soccer::Role::Unknown:
            type_str = "Unknown";
            break;

        case Immortals::Common::Soccer::Role::Gk:
            type_str = "Gk";
            break;
        case Immortals::Common::Soccer::Role::Def1:
            type_str = "Def1";
            break;
        case Immortals::Common::Soccer::Role::Def2:
            type_str = "Def2";
            break;
        case Immortals::Common::Soccer::Role::Mid1:
            type_str = "Mid1";
            break;
        case Immortals::Common::Soccer::Role::Mid2:
            type_str = "Mid2";
            break;
        case Immortals::Common::Soccer::Role::Mid3:
            type_str = "Mid3";
            break;
        case Immortals::Common::Soccer::Role::Mid4:
            type_str = "Mid4";
            break;
        case Immortals::Common::Soccer::Role::Mid5:
            type_str = "Mid5";
            break;
        case Immortals::Common::Soccer::Role::Mid6:
            type_str = "Mid6";
            break;
        case Immortals::Common::Soccer::Role::Mid7:
            type_str = "Mid7";
            break;

        case Immortals::Common::Soccer::Role::Attack:
            type_str = "Attack";
            break;
        }

        return fmt::format_to(t_ctx.out(), "{}", type_str);
    }
};

template <>
struct fmt::formatter<Immortals::Common::Soccer::OneTouchType> : fmt::formatter<std::string>
{
    auto format(const Immortals::Common::Soccer::OneTouchType t_type, format_context &t_ctx) const
    {
        const char *type_str = "Unknown";
        switch (t_type)
        {
        case Immortals::Common::Soccer::OneTouchType::OneTouch:
            type_str = "One Touch";
            break;
        case Immortals::Common::Soccer::OneTouchType::Shirje:
            type_str = "Shirje";
            break;
        case Immortals::Common::Soccer::OneTouchType::Gool:
            type_str = "Gool";
            break;
        case Immortals::Common::Soccer::OneTouchType::Allaf:
            type_str = "Allaf";
            break;
        }

        return fmt::format_to(t_ctx.out(), "{}", type_str);
    }
};

#endif
