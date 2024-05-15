#pragma once

#include <time/time_point.h>

namespace Tyr::Sender
{
struct Command
{
    int vision_id = -1;

    Common::Vec3 motion;
    bool         halted = true;

    Common::Angle current_angle;
    Common::Angle target_angle;

    int shoot    = 0;
    int chip     = 0;
    int dribbler = 0;

    Command() = default;

    Command(const Protos::Immortals::Command &t_command)
    {
        vision_id = t_command.vision_id();

        motion = t_command.motion();
        halted = t_command.halted();

        current_angle = t_command.current_angle();
        target_angle  = t_command.target_angle();

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

    std::vector<Command> command;

    CommandsWrapper() = default;

    CommandsWrapper(const Protos::Immortals::CommandsWrapper &t_wrapper)
    {
        time = Common::TimePoint::fromMicroseconds(t_wrapper.time());

        for (const auto &command : t_wrapper.command())
            this->command.emplace_back(command);
    }

    void fillProto(Protos::Immortals::CommandsWrapper *const t_wrapper) const
    {
        t_wrapper->set_time(time.microseconds());

        for (const auto &command : command)
            command.fillProto(t_wrapper->add_command());
    }
};
} // namespace Tyr::Sender