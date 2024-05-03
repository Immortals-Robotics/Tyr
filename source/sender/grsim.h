#pragma once

#include "sender.h"

namespace Tyr::Sender
{
class Grsim final : public ISender
{
private:
    std::unique_ptr<Common::UdpServer> socket;
    Common::NetworkAddress             address;

    Protos::grSim_Packet packet;

public:
    Grsim(Common::NetworkAddress address);
    ~Grsim() = default;

    void queueCommand(const Command &command) override;
    bool flush() override;
};
} // namespace Tyr::Sender
