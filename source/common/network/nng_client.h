#pragma once

#include "../setting.h"
#include "../time/time_point.h"
#include "nng_message.h"

namespace Tyr::Common
{
class NngClient
{
public:
    NngClient(std::string_view t_url);

    bool receive(TimePoint *t_time, google::protobuf::MessageLite *t_message);

    NngMessage receiveRaw();

private:
    nng_socket m_socket;
    nng_dialer m_dialer;
};
} // namespace Tyr::Common
