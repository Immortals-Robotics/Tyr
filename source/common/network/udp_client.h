#pragma once

#include "../config/config.h"

namespace Immortals::Common
{
class UdpClient
{
public:
    explicit UdpClient(const NetworkAddress &t_address);

    void updateAddress(const NetworkAddress &t_address);

    bool receive(google::protobuf::MessageLite *t_message);

    bool receiveRaw(std::span<char> *t_data);

    [[nodiscard]] NetworkAddress getListenEndpoint() const
    {
        return NetworkAddress{m_listen_endpoint};
    }

    [[nodiscard]] NetworkAddress getLastReceiveEndpoint() const
    {
        return NetworkAddress{m_last_receive_endpoint};
    }

    [[nodiscard]] asio::ip::address getAddress() const
    {
        return m_address;
    }

    [[nodiscard]] bool isConnected() const
    {
        return m_socket->is_open();
    }

private:
    asio::ip::udp::endpoint m_listen_endpoint;
    asio::ip::address       m_address;

    asio::ip::udp::endpoint m_last_receive_endpoint;

    std::unique_ptr<asio::io_context>      m_context;
    std::unique_ptr<asio::ip::udp::socket> m_socket;

    std::array<char, Config::Network::kMaxUdpPacketSize> m_buffer = {};
};
} // namespace Immortals::Common
