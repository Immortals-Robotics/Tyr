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
    ~Grsim() = default;

// TODO: fix and remove
#if GRSIM_FIXED
    void sendData(const Robot *const robots, const int robot_count, bool color);
#endif
};
} // namespace Tyr::Sender