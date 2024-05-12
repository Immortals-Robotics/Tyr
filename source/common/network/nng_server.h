#pragma once

#include "../setting.h"
#include "nng_message.h"

namespace Tyr::Common
{
class NngServer
{
public:
    NngServer(std::string_view t_url);

    // Serializes the protobuf message to the internal buffer and sends it
    bool send(const google::protobuf::MessageLite &t_message);

    // Sends the first t_size bytes of the internal bugffer
    bool send(const NngMessage& t_message);

private:
    nng_socket m_socket;
};
} // namespace Tyr::Common
