#include "nng_client.h"

namespace Tyr::Common
{
NngClient::NngClient(const std::string_view t_url)
{
    int result;

    result = nng_sub_open(&m_socket);
    if (result != 0)
    {
        logCritical("Failed to open nng sub socket: {}", result);
    }

    // TODO: take filter as an input
    result = nng_setopt(m_socket, NNG_OPT_SUB_SUBSCRIBE, "", 0);
    if (result != 0)
    {
        logCritical("Failed to set nng socket subscribe filter: {}", result);
    }

    const std::string url_null_terminated{t_url.data(), t_url.size()};
    result = nng_dial(m_socket, url_null_terminated.c_str(), nullptr, 0);
    if (result != 0)
    {
        logCritical("Failed to dial nng sub socket: {}", result);
    }
}

bool NngClient::receive(google::protobuf::MessageLite *const t_message)
{
    std::span<char> data;
    if (!receiveRaw(&data))
        return false;

    return t_message->ParseFromArray(data.data(), data.size());
}

bool NngClient::receiveRaw(std::span<char> *const t_data)
{
    asio::error_code error;

    size_t received_size = m_buffer.size();

    const int result = nng_recv(m_socket, m_buffer.data(), &received_size, 0 /*NNG_FLAG_NONBLOCK*/);
    if (result != 0)
    {
        logCritical("Failed to receive from nng sub socket: {}", result);
        return false;
    }

    *t_data = std::span<char>(m_buffer.data(), received_size);
    return true;
}

} // namespace Tyr::Common
