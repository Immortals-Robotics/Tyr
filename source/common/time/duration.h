#pragma once

namespace Tyr::Common
{
struct Duration
{
    inline static Duration fromMilliseconds(const uint64_t t_duration)
    {
        return {.duration = std::chrono::milliseconds{t_duration}};
    }

    inline static Duration fromMicroseconds(const uint64_t t_duration)
    {
        return {.duration = std::chrono::microseconds{t_duration}};
    }

    inline static Duration fromSeconds(const float t_duration)
    {
        const auto duration = std::chrono::duration<float>{t_duration};
        return {.duration = std::chrono::round<std::chrono::system_clock::duration>(duration)};
    }

    inline uint64_t milliseconds() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }

    inline uint64_t microseconds() const
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    }

    inline float seconds() const
    {
        return std::chrono::duration_cast<std::chrono::duration<float>>(duration).count();
    }

    std::chrono::system_clock::duration duration = {};
};

inline auto format_as(const Duration t_duration)
{
    return t_duration.duration;
}

} // namespace Tyr::Common
