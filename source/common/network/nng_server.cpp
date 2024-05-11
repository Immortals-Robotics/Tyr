#include "nng_server.h"

namespace Tyr::Common
{
NngServer::NngServer(const std::string_view t_url)
{
    int result;

    result = nng_pub_open(&m_socket);
    if (result != 0)
    {
        logCritical("Failed to open nng pub socket: {}", result);
    }

    const std::string url_null_terminated{t_url.data(), t_url.size()};

    result = nng_listen(m_socket, url_null_terminated.c_str(), nullptr, 0);
    if (result != 0)
    {
        logCritical("Failed to listen nng pub socket: {}", result);
    }
}

bool NngServer::send(const google::protobuf::MessageLite &t_message)
{
    if (!t_message.SerializeToArray(m_buffer.data(), m_buffer.size()))
    {
        logError("Failed to serializeToArray (Maybe need to adjust Setting::kMaxUdpPacketSize)");
        return false;
    }

    const size_t serialized_size = t_message.ByteSizeLong();
    return send(serialized_size);
}

bool NngServer::send(const size_t t_size)
{
    const int result = nng_send(m_socket, m_buffer.data(), t_size, 0);
    if (result != 0)
    {
        logError("Nng send of {} bytes failed: {}", t_size, result);
        return false;
    }

    return true;
}
} // namespace Tyr::Common
