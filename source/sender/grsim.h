#pragma once

#include "base.h"

namespace Tyr::Sender
{
class Grsim final : public Base
{
private:
    std::unique_ptr<Common::UdpServer> m_socket;

    Protos::GrSim::Packet m_packet;

    void queueCommand(const Command &t_command);

public:
    Grsim();
    ~Grsim() = default;

    bool send(const CommandsWrapper &t_wrapper) override;
};
} // namespace Tyr::Sender
