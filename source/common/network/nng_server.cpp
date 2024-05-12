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
    const NngMessage message{t_message.ByteSizeLong()};

    if (!t_message.SerializeToArray(message.data(), message.size()))
    {
        logError("Failed to serializeToArray (Maybe need to adjust Setting::kMaxUdpPacketSize)");
        return false;
    }

    return send(message);
}

bool NngServer::send(const NngMessage &t_message)
{
    const int result = nng_send(m_socket, t_message.data(), t_message.size(), 0);
    if (result != 0)
    {
        logError("Nng send of {} bytes failed: {}", t_message.size(), result);
        return false;
    }

    return true;
}
} // namespace Tyr::Common
