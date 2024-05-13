#pragma once

#include "base.h"
#include "command.h"

namespace Tyr::Sender
{
class Hub
{
public:
    Hub()
    {
        m_client = std::make_unique<Common::NngClient>(Common::setting().commands_url);
    }

    template <typename Sender, typename... Args>
    void registerSender(Args &&...args)
    {
        m_senders.push_back(std::make_unique<Sender>(std::forward<Args>(args)...));
    }

    bool send()
    {
        bool result = true;

        for (auto &sender : m_senders)
        {
            if (!sender->send(m_wrapper))
                result = false;
        }

        return result;
    }

    bool receive()
    {
        Protos::Immortals::CommandsWrapper pb_wrapper;
        if (!m_client->receive(nullptr, &pb_wrapper))
            return false;

        m_wrapper = CommandsWrapper(pb_wrapper);
        return true;
    }

private:
    std::unique_ptr<Common::NngClient> m_client;
    std::vector<std::unique_ptr<Base>> m_senders;

    CommandsWrapper m_wrapper;
};
} // namespace Tyr::Sender
