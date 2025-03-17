#include "nng_client.h"

namespace Immortals::Common
{
NngClient::NngClient(const std::string_view t_url)
{
    int result;

    result = nng_sub_open(&m_socket);
    if (result != 0)
    {
        logCritical("Failed to open nng sub socket: {}", nng_strerror(result));
    }

    // TODO: take filter as an input
    result = nng_socket_set(m_socket, NNG_OPT_SUB_SUBSCRIBE, "", 0);
    if (result != 0)
    {
        logCritical("Failed to set nng socket subscribe filter: {}", nng_strerror(result));
    }

    const std::string url_null_terminated{t_url.data(), t_url.size()};
    result = nng_dial(m_socket, url_null_terminated.c_str(), &m_dialer, NNG_FLAG_NONBLOCK);
    if (result != 0)
    {
        logCritical("Failed to dial \"{}\" with nng sub socket: {}", t_url, nng_strerror(result));
    }
}

bool NngClient::receive(google::protobuf::MessageLite *const t_message, TimePoint *const t_time, const bool t_drain)
{
    const NngMessage message = receiveRaw(t_drain);

    if (message.size() == 0)
        return false;

    if (t_time != nullptr)
        *t_time = message.time();

    return t_message->ParseFromArray(message.data(), message.size());
}

NngMessage NngClient::receiveRaw(const bool t_drain)
{
    char  *buffer = nullptr;
    size_t size   = 0;

    do
    {
        char  *new_buffer = nullptr;
        size_t new_size   = 0;

        const int result = nng_recv(m_socket, &new_buffer, &new_size, NNG_FLAG_ALLOC | NNG_FLAG_NONBLOCK);
        if (result != 0)
        {
            if (result != NNG_EAGAIN)
                logCritical("Failed to receive from nng sub socket: {}", nng_strerror(result));
            break;
        }
        else
        {
            buffer = new_buffer;
            size   = new_size;
        }
    }
    while (t_drain);

    return {buffer, size};
}

} // namespace Immortals::Common
