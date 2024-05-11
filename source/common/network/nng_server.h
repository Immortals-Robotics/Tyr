#pragma once

#include "../setting.h"

namespace Tyr::Common
{
class NngServer
{
public:
    NngServer(std::string_view t_url);

    // Serializes the protobuf message to the internal buffer and sends it
    bool send(const google::protobuf::MessageLite &t_message);

    // Sends the first t_size bytes of the internal bugffer
    bool send(size_t t_size);

    std::span<char> getBuffer()
    {
        return m_buffer;
    }

private:
    nng_socket m_socket;

    std::array<char, Setting::kMaxUdpPacketSize> m_buffer = {};
};
} // namespace Tyr::Common
