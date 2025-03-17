#pragma once

#include "../config/config.h"

namespace Tyr::Common
{
class UdpServer
{
public:
    UdpServer();

    // Serializes the protobuf message to the internal buffer and sends it
    bool send(const google::protobuf::MessageLite &t_message, const NetworkAddress &t_address);

    // Sends the first t_size bytes of the internal bugffer
    bool send(size_t t_size, const NetworkAddress &t_address);

    std::span<char> getBuffer()
    {
        return m_buffer;
    }

    [[nodiscard]] NetworkAddress getListenEndpoint() const
    {
        return NetworkAddress{m_listen_endpoint};
    }

private:
    asio::ip::udp::endpoint m_listen_endpoint;

    std::unique_ptr<asio::io_context>      m_context;
    std::unique_ptr<asio::ip::udp::socket> m_socket;

    std::array<char, Config::Network::kMaxUdpPacketSize> m_buffer = {};
};
} // namespace Tyr::Common
