#include "nng_server.h"

namespace Immortals::Common
{
NngServer::NngServer(const std::string_view t_url)
{
    int result;

    result = nng_pub_open(&m_socket);
    if (result != 0)
    {
        logCritical("Failed to open nng pub socket: {}", nng_strerror(result));
    }

    const std::string url_null_terminated{t_url.data(), t_url.size()};

    result = nng_listen(m_socket, url_null_terminated.c_str(), &m_listener, 0);
    if (result != 0)
    {
        logCritical("Failed to listen to \"{}\" with nng pub socket: {}", t_url, nng_strerror(result));
    }
}

bool NngServer::send(const TimePoint &t_time, const google::protobuf::MessageLite &t_message)
{
    NngMessage message{t_message.ByteSizeLong()};

    *message.mutableTime() = t_time.microseconds();

    if (!t_message.SerializeToArray(message.data(), message.size()))
    {
        logError("Failed to serializeToArray (Maybe need to adjust Setting::kMaxUdpPacketSize)");
        return false;
    }

    return sendRaw(message);
}

bool NngServer::sendRaw(const NngMessage &t_message)
{
    const int result = nng_send(m_socket, t_message.m_data.data(), t_message.m_data.size(), 0);
    if (result != 0)
    {
        logError("Nng send of {} bytes failed: {}", t_message.m_data.size(), nng_strerror(result));
        return false;
    }

    return true;
}
} // namespace Immortals::Common
