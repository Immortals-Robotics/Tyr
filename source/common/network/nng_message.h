#pragma once

namespace Tyr::Common
{
struct NngMessage : public std::span<char>
{
    NngMessage() : NngMessage(nullptr, 0)
    {}

    NngMessage(const size_t t_size) : NngMessage((char *) nng_alloc(t_size), t_size)
    {}

    ~NngMessage()
    {
        if (data() != nullptr)
        {
            nng_free(data(), size());
        }
    }

    NngMessage(const NngMessage &) = delete;

private:
    NngMessage(char *const t_buffer, const size_t t_size) : std::span<char>(t_buffer, t_size)
    {}

    friend class NngClient;
    friend class NngServer;
};
} // namespace Tyr::Common
