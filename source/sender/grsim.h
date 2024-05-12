#pragma once

#include "sender.h"

namespace Tyr::Sender
{
class Grsim final : public Base
{
private:
    std::unique_ptr<Common::UdpServer> m_socket;

    Protos::grSim_Packet m_packet;

    void queueCommand(const Command &t_command);

public:
    Grsim();
    ~Grsim() = default;

    bool send() override;
};
} // namespace Tyr::Sender
