#include "simulator.h"

namespace Tyr::Sender
{
Simulator::Simulator() : Base()
{
    m_socket = std::make_unique<Common::UdpServer>();
}

void Simulator::queueCommand(const Command &command)
{
    Protos::Ssl::Simulation::RobotCommand *proto_command = m_packet.add_robot_commands();
    proto_command->set_id(command.vision_id);

    Protos::Ssl::Simulation::RobotMoveCommand *const proto_move_cmd = proto_command->mutable_move_command();

    Protos::Ssl::Simulation::MoveLocalVelocity *const local_velocity = proto_move_cmd->mutable_local_velocity();

    const Common::Vec2 local_vel = command.motion.rotated(Common::Angle::fromDeg(90.0f) - command.current_angle);
    local_velocity->set_forward(local_vel.y / 1000.0f);
    local_velocity->set_left(-local_vel.x / 1000.0f);

    float w = (command.target_angle - command.current_angle).deg() / 10.0f;
    local_velocity->set_angular(w);

    if (command.shoot > 0)
    {
        proto_command->set_kick_speed(command.shoot / 10.f);
        proto_command->set_kick_angle(0.0f);
    }
    else if (command.chip > 0)
    {
        proto_command->set_kick_speed(command.chip / 20.0f);
        proto_command->set_kick_angle(45.0f);
    }
    else
    {
        proto_command->set_kick_speed(0.0f);
        proto_command->set_kick_angle(0.0f);
    }

    proto_command->set_dribbler_speed(command.dribbler);
}

bool Simulator::send(const CommandsWrapper &t_wrapper)
{
    m_packet.Clear();

    for (const auto &command : t_wrapper.commands)
        queueCommand(command);

    const Common::Config::Network::Address &address = Common::setting().common.our_color == Common::TeamColor::Yellow
                                                          ? Common::setting().network.yellow_robot_simulation_address
                                                          : Common::setting().network.blue_robot_simulation_address;
    return m_socket->send(m_packet, address);
}

} // namespace Tyr::Sender
