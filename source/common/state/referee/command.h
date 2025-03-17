#pragma once

namespace Immortals::Common::Referee
{
struct Command
{
    enum class Type
    {
        None = -1,

        Halt                 = 0,
        Stop                 = 1,
        NormalStart          = 2,
        ForceStart           = 3,
        PrepareKickoffYellow = 4,
        PrepareKickoffBlue   = 5,
        PreparePenaltyYellow = 6,
        PreparePenaltyBlue   = 7,
        DirectFreeYellow     = 8,
        DirectFreeBlue       = 9,
        TimeoutYellow        = 12,
        TimeoutBlue          = 13,
        BallPlacementYellow  = 16,
        BallPlacementBlue    = 17,
    };

    Command() = default;

    explicit Command(const Protos::Ssl::Gc::Referee &t_packet)
    {
        id = t_packet.command_counter();

        time_issued = TimePoint::fromMicroseconds(t_packet.command_timestamp());
        time_sent   = TimePoint::fromMicroseconds(t_packet.packet_timestamp());

        if (t_packet.has_source_identifier())
        {
            source_uuid = t_packet.source_identifier();
        }

        type = static_cast<Type>(t_packet.command());

        if (t_packet.has_next_command())
        {
            next = static_cast<Type>(t_packet.next_command());
        }
    }

    explicit Command(const Protos::Immortals::Referee::Command &t_command)
    {
        id = t_command.id();

        time_issued = TimePoint::fromMicroseconds(t_command.time_issued());
        time_sent   = TimePoint::fromMicroseconds(t_command.time_sent());

        source_uuid = t_command.source_uuid();

        type = static_cast<Type>(t_command.type());
        next = static_cast<Type>(t_command.next());
    }

    void fillProto(Protos::Immortals::Referee::Command *const t_command) const
    {
        t_command->set_id(id);

        t_command->set_time_issued(time_issued.microseconds());
        t_command->set_time_sent(time_sent.microseconds());

        t_command->set_source_uuid(source_uuid);

        t_command->set_type(static_cast<Protos::Immortals::Referee::Command_Type>(type));
        t_command->set_next(static_cast<Protos::Immortals::Referee::Command_Type>(next));
    }

    unsigned id = 0;

    TimePoint time_issued;
    TimePoint time_sent;

    std::string source_uuid;

    Type type = Type::None;
    // The command that will be issued after the current stoppage and ball placement to continue the game.
    Type next = Type::None;
};

} // namespace Immortals::Common::Referee

#if FEATURE_LOGGING
template <>
struct fmt::formatter<Immortals::Common::Referee::Command::Type> : fmt::formatter<std::string>
{
    auto format(const Immortals::Common::Referee::Command::Type t_type, format_context &t_ctx) const
    {
        const char *type_str = "Unknown";
        switch (t_type)
        {
        case Immortals::Common::Referee::Command::Type::None:
            type_str = "None";
            break;
        case Immortals::Common::Referee::Command::Type::Halt:
            type_str = "Halt";
            break;
        case Immortals::Common::Referee::Command::Type::Stop:
            type_str = "Stop";
            break;
        case Immortals::Common::Referee::Command::Type::NormalStart:
            type_str = "NormalStart";
            break;
        case Immortals::Common::Referee::Command::Type::ForceStart:
            type_str = "ForceStart";
            break;
        case Immortals::Common::Referee::Command::Type::PrepareKickoffYellow:
            type_str = "PrepareKickoffYellow";
            break;
        case Immortals::Common::Referee::Command::Type::PrepareKickoffBlue:
            type_str = "PrepareKickoffBlue";
            break;
        case Immortals::Common::Referee::Command::Type::PreparePenaltyYellow:
            type_str = "PreparePenaltyYellow";
            break;
        case Immortals::Common::Referee::Command::Type::PreparePenaltyBlue:
            type_str = "PreparePenaltyBlue";
            break;
        case Immortals::Common::Referee::Command::Type::DirectFreeYellow:
            type_str = "DirectFreeYellow";
            break;
        case Immortals::Common::Referee::Command::Type::DirectFreeBlue:
            type_str = "DirectFreeBlue";
            break;
        case Immortals::Common::Referee::Command::Type::TimeoutYellow:
            type_str = "TimeoutYellow";
            break;
        case Immortals::Common::Referee::Command::Type::TimeoutBlue:
            type_str = "TimeoutBlue";
            break;
        case Immortals::Common::Referee::Command::Type::BallPlacementYellow:
            type_str = "BallPlacementYellow";
            break;
        case Immortals::Common::Referee::Command::Type::BallPlacementBlue:
            type_str = "BallPlacementBlue";
            break;
        }

        return fmt::format_to(t_ctx.out(), "{}", type_str);
    }
};

template <>
struct fmt::formatter<Immortals::Common::Referee::Command> : fmt::formatter<std::string>
{
    auto format(const Immortals::Common::Referee::Command &t_command, format_context &t_ctx) const
    {
        return fmt::format_to(t_ctx.out(), "[{}] [{}] {}", t_command.id, t_command.time_issued, t_command.type);
    }
};
#endif
