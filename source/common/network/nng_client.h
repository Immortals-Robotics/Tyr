#pragma once

#include "../time/time_point.h"
#include "nng_message.h"

namespace Immortals::Common
{
class NngClient
{
public:
    NngClient(std::string_view t_url);

    bool receive(google::protobuf::MessageLite *t_message, TimePoint *t_time = nullptr, bool t_drain = false);

    NngMessage receiveRaw(bool t_drain = false);

private:
    nng_socket m_socket;
    nng_dialer m_dialer;
};
} // namespace Immortals::Common
