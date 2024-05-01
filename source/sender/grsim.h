#pragma once

#include "command.h"

namespace Tyr::Sender
{
class Grsim
{
private:
    std::unique_ptr<Common::UdpServer> socket;
    Common::NetworkAddress             address;

public:
    Grsim(Common::NetworkAddress address);
    ~Grsim() = default;

    void sendData(const std::vector<Command>& commands);
};
} // namespace Tyr::Sender