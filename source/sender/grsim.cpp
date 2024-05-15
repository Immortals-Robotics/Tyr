#include "grsim.h"

namespace Tyr::Sender
{
Grsim::Grsim() : Base()
{
    m_socket = std::make_unique<Common::UdpServer>();
}

void Grsim::queueCommand(const Command &command)
{
    Protos::GrSim::RobotCommand *proto_command = m_packet.mutable_commands()->add_robot_commands();
    proto_command->set_id(command.vision_id);

    proto_command->set_wheelsspeed(false);

    const Common::Vec3 motion = command.motion;

    float robot_ang = (Common::Angle::fromDeg(90.0f) - command.current_angle).rad();

    float new_VelX = motion.x * cos(robot_ang) - motion.y * sin(robot_ang);
    float new_VelY = motion.x * sin(robot_ang) + motion.y * cos(robot_ang);

    proto_command->set_veltangent(new_VelY / 20.0);
    proto_command->set_velnormal(-new_VelX / 20.0);

    float w = (command.target_angle - command.current_angle).deg();
    w /= 10.0f;

    proto_command->set_velangular(w);

    if (command.shoot > 0)
    {
        proto_command->set_kickspeedx(command.shoot / 10.f);
        proto_command->set_kickspeedz(0);
    }
    else if (command.chip > 0)
    {
        const float        chip_mag = command.chip / 20.0f;
        const Common::Vec2 chip_dir = Common::Angle::fromDeg(45.0f).toUnitVec();
        proto_command->set_kickspeedx(chip_mag * chip_dir.x);
        proto_command->set_kickspeedz(chip_mag * chip_dir.y);
    }
    else
    {
        proto_command->set_kickspeedx(.0f);
        proto_command->set_kickspeedz(.0f);
    }

    proto_command->set_spinner(command.dribbler);
}

bool Grsim::send(const CommandsWrapper &t_wrapper)
{
    m_packet.Clear();

    for (const auto &command : t_wrapper.command)
        queueCommand(command);

    m_packet.mutable_commands()->set_isteamyellow(Common::setting().our_color == Common::TeamColor::Yellow);
    m_packet.mutable_commands()->set_timestamp(0.0);

    return m_socket->send(m_packet, Common::setting().grsim_address);
}

} // namespace Tyr::Sender