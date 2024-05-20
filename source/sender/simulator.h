#pragma once

#include "base.h"

namespace Tyr::Sender
{
class Simulator final : public Base
{
private:
    std::unique_ptr<Common::UdpServer> m_socket;

    Protos::Ssl::Simulation::RobotControl m_packet;

    void queueCommand(const Command &t_command);

public:
    Simulator();
    ~Simulator() = default;

    bool send(const CommandsWrapper &t_wrapper) override;
};
} // namespace Tyr::Sender
