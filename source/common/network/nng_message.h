#pragma once

#include "../time/time_point.h"

namespace Tyr::Common
{
struct NngMessage
{
    NngMessage() : NngMessage(nullptr, 0)
    {}

    NngMessage(const size_t t_size)
        : NngMessage((char *) nng_alloc(t_size + sizeof(TimeStamp)), t_size + sizeof(TimeStamp))
    {}

    ~NngMessage()
    {
        if (m_data.data() != nullptr)
        {
            nng_free(m_data.data(), m_data.size());
        }
    }

    NngMessage(const NngMessage &) = delete;

    TimePoint time() const
    {
        return m_data.size() >= sizeof(TimeStamp) ? *((TimeStamp *) m_data.data()) : 0;
    }

    TimeStamp *mutableTime()
    {
        return m_data.size() >= sizeof(TimeStamp) ? (TimeStamp *) m_data.data() : nullptr;
    }

    char *data() const
    {
        return m_data.data() != nullptr ? m_data.data() + sizeof(TimeStamp) : nullptr;
    }

    size_t size() const
    {
        return m_data.size() >= sizeof(TimeStamp) ? m_data.size() - sizeof(TimeStamp) : 0;
    }

private:
    NngMessage(char *const t_buffer, const size_t t_size) : m_data(t_buffer, t_size)
    {}

    friend class NngClient;
    friend class NngServer;

    std::span<char> m_data;
};
} // namespace Tyr::Common
