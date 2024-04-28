#pragma once

#include <string>

#include <network/udp_server.h>

namespace Tyr::Sender
{
class Grsim
{
private:
    std::unique_ptr<Common::UdpServer> socket;
    Common::NetworkAddress             address;

public:
    Grsim(Common::NetworkAddress address);
    ~Grsim();
    void sendData(const Robot *const robots, const int robot_count, bool color);
};
} // namespace Tyr::Sender