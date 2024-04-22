#pragma once

#if defined(_WIN32)           
	#define NOGDI             // All GDI defines and routines
	#define NOUSER            // All USER defines and routines
#endif

#include <asio.hpp>

#if defined(_WIN32)           // raylib uses these names as function parameters
	#undef near
	#undef far
#endif

#include <google/protobuf/message_lite.h>

#include "../setting.h"

namespace Immortals
{
class UdpClient
{
public:
    explicit UdpClient(const NetworkAddress &t_address);

    bool receive(google::protobuf::MessageLite *t_message);

    [[nodiscard]] asio::ip::udp::endpoint getListenEndpoint() const
    {
        return m_listen_endpoint;
    }

    [[nodiscard]] asio::ip::udp::endpoint getLastReceiveEndpoint() const
    {
        return m_last_receive_endpoint;
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

    std::array<char, Setting::kMaxUdpPacketSize> m_buffer = {};
};
} // namespace Immortals
