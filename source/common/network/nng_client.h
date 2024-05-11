#pragma once

#include "../setting.h"

namespace Tyr::Common
{
class NngClient
{
public:
    NngClient(std::string_view t_url);

    bool receive(google::protobuf::MessageLite *t_message);

    bool receiveRaw(std::span<char> *t_data);

private:
    nng_socket m_socket;

    std::array<char, Setting::kMaxUdpPacketSize> m_buffer = {};
};
} // namespace Tyr::Common
