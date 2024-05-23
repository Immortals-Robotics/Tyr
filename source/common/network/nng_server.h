#pragma once

#include "../time/time_point.h"
#include "nng_message.h"

namespace Tyr::Common
{
class NngServer
{
public:
    NngServer(std::string_view t_url);

    // Serializes the protobuf message to the internal buffer and sends it
    bool send(const TimePoint &t_time, const google::protobuf::MessageLite &t_message);

    bool sendRaw(const NngMessage &t_message);

private:
    nng_socket   m_socket;
    nng_listener m_listener;
};
} // namespace Tyr::Common
