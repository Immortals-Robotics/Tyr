#include "udp_client.h"

namespace Immortals::Common
{
UdpClient::UdpClient(const NetworkAddress &t_address)
{
    m_context = std::make_unique<asio::io_context>();
    m_socket  = std::make_unique<asio::ip::udp::socket>(*m_context);

    updateAddress(t_address);
}

void UdpClient::updateAddress(const NetworkAddress &t_address)
{
    if (m_socket->is_open())
    {
        m_socket->close();
    }

    m_address         = asio::ip::make_address(t_address.ip);
    m_listen_endpoint = asio::ip::udp::endpoint{asio::ip::udp::v4(), t_address.port};

    m_socket->open(m_listen_endpoint.protocol());
    m_socket->set_option(asio::ip::udp::socket::reuse_address(true));
    m_socket->bind(m_listen_endpoint);
    if (m_address.is_multicast())
    {
        m_socket->set_option(asio::ip::multicast::join_group(m_address));
    }
    m_socket->non_blocking(true);
}

bool UdpClient::receive(google::protobuf::MessageLite *const t_message)
{
    std::span<char> data;
    if (!receiveRaw(&data))
        return false;

    return t_message->ParseFromArray(data.data(), data.size());
}

bool UdpClient::receiveRaw(std::span<char> *const t_data)
{
    asio::error_code error;

    const size_t received_size = m_socket->receive_from(asio::buffer(m_buffer), m_last_receive_endpoint, 0, error);

    if (error)
    {
        if (error != asio::error::would_block)
            logError("Udp receive failed with [{}]: {}", error.value(), error.message());
        return false;
    }

    *t_data = std::span<char>(m_buffer.data(), received_size);
    return true;
}

} // namespace Immortals::Common
