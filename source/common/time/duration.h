#pragma once

namespace Tyr::Common
{
struct Duration
{
    static Duration fromMilliseconds(const uint64_t t_duration)
    {
        return {std::chrono::milliseconds{t_duration}};
    }

    static Duration fromMicroseconds(const uint64_t t_duration)
    {
        return {std::chrono::microseconds{t_duration}};
    }

    static Duration fromSeconds(const float t_duration)
    {
        const auto duration = std::chrono::duration<float>{t_duration};
        return {std::chrono::round<std::chrono::system_clock::duration>(duration)};
    }

    uint64_t milliseconds() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }

    uint64_t microseconds() const
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    }

    float seconds() const
    {
        return std::chrono::duration_cast<std::chrono::duration<float>>(duration).count();
    }

    bool operator==(const Duration &t_other) const
    {
        return duration == t_other.duration;
    }

    bool operator!=(const Duration &t_other) const
    {
        return duration != t_other.duration;
    }

    bool operator<(const Duration &t_other) const
    {
        return duration < t_other.duration;
    }

    bool operator>(const Duration &t_other) const
    {
        return duration > t_other.duration;
    }

    bool operator<=(const Duration &t_other) const
    {
        return duration <= t_other.duration;
    }

    bool operator>=(const Duration &t_other) const
    {
        return duration >= t_other.duration;
    }

    Duration operator+(const Duration t_other) const
    {
        return {duration + t_other.duration};
    }

    Duration operator-(const Duration t_other) const
    {
        return {duration - t_other.duration};
    }

    Duration &operator-=(const Duration &t_other)
    {
        duration -= t_other.duration;
        return *this;
    }

    Duration &operator+=(const Duration &t_other)
    {
        duration += t_other.duration;
        return *this;
    }

    std::chrono::system_clock::duration duration = {};
};

inline auto format_as(const Duration t_duration)
{
    return t_duration.duration;
}

} // namespace Tyr::Common
