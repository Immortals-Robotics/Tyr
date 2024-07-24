#pragma once

namespace Tyr::Sender
{
struct Command
{
    int vision_id = -1;

    Common::Vec2 motion;
    bool         halted = true;

    Common::Angle current_angle;
    Common::Angle target_angle;

    float shoot    = 0;
    float chip     = 0;
    float dribbler = 0;

    Command() = default;

    explicit Command(const Protos::Immortals::Command &t_command)
    {
        vision_id = t_command.vision_id();

        motion = Common::Vec2{t_command.motion()};
        halted = t_command.halted();

        current_angle = Common::Angle{t_command.current_angle()};
        target_angle  = Common::Angle{t_command.target_angle()};

        shoot    = t_command.shoot();
        chip     = t_command.chip();
        dribbler = t_command.dribbler();
    }

    void fillProto(Protos::Immortals::Command *const t_command) const
    {
        t_command->set_vision_id(vision_id);

        motion.fillProto(t_command->mutable_motion());
        t_command->set_halted(halted);

        current_angle.fillProto(t_command->mutable_current_angle());
        target_angle.fillProto(t_command->mutable_target_angle());

        t_command->set_shoot(shoot);
        t_command->set_chip(chip);
        t_command->set_dribbler(dribbler);
    }
};

struct CommandsWrapper
{
    Common::TimePoint time;

    std::vector<Command> commands;

    CommandsWrapper() = default;

    CommandsWrapper(const Protos::Immortals::CommandsWrapper &t_wrapper)
    {
        time = Common::TimePoint::fromMicroseconds(t_wrapper.time());

        for (const auto &pb_command : t_wrapper.command())
            commands.emplace_back(pb_command);
    }

    void fillProto(Protos::Immortals::CommandsWrapper *const t_wrapper) const
    {
        t_wrapper->set_time(time.microseconds());

        for (const auto &command : commands)
            command.fillProto(t_wrapper->add_command());
    }
};
} // namespace Tyr::Sender
