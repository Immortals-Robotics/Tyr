#pragma once

#include "../time/time_point.h"

namespace Immortals::Common
{
struct NngMessage
{
    NngMessage() : NngMessage(nullptr, 0)
    {}

    NngMessage(const size_t t_size)
        : NngMessage(static_cast<char *>(nng_alloc(t_size + sizeof(uint64_t))), t_size + sizeof(uint64_t))
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
        const uint64_t timestamp = m_data.size() >= sizeof(uint64_t) ? *((uint64_t *) m_data.data()) : 0;
        return TimePoint::fromMicroseconds(timestamp);
    }

    uint64_t *mutableTime()
    {
        return m_data.size() >= sizeof(uint64_t) ? (uint64_t *) m_data.data() : nullptr;
    }

    char *data() const
    {
        return m_data.data() != nullptr ? m_data.data() + sizeof(uint64_t) : nullptr;
    }

    size_t size() const
    {
        return m_data.size() >= sizeof(uint64_t) ? m_data.size() - sizeof(uint64_t) : 0;
    }

private:
    NngMessage(char *const t_buffer, const size_t t_size) : m_data(t_buffer, t_size)
    {}

    friend class NngClient;
    friend class NngServer;

    std::span<char> m_data;
};
} // namespace Immortals::Common
