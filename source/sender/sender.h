#pragma once

#include "command.h"

namespace Tyr::Sender
{
class Base
{
public:
    Base()
    {
        m_client = std::make_unique<Common::NngClient>(Common::setting().commands_url);
    }

    virtual ~Base() = default;

    virtual bool send() = 0;

    bool receive()
    {
        Protos::Immortals::CommandsWrapper pb_wrapper;
        if (!m_client->receive(&pb_wrapper))
            return false;

        m_wrapper = CommandsWrapper(pb_wrapper);
        return true;
    }

private:
    std::unique_ptr<Common::NngClient> m_client;

protected:
    CommandsWrapper m_wrapper;
};
} // namespace Tyr::Sender
