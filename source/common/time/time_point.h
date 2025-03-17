#pragma once

#include "duration.h"

namespace Immortals::Common
{
struct TimePoint
{
    static TimePoint fromMilliseconds(const uint64_t t_timestamp)
    {
        return {std::chrono::system_clock::time_point(std::chrono::milliseconds{t_timestamp})};
    }

    static TimePoint fromMicroseconds(const uint64_t t_timestamp)
    {
        return {std::chrono::system_clock::time_point(std::chrono::microseconds{t_timestamp})};
    }

    static TimePoint fromSeconds(const float t_timestamp)
    {
        const auto duration = std::chrono::duration<float>{t_timestamp};
        return {std::chrono::system_clock::time_point(std::chrono::round<std::chrono::microseconds>(duration))};
    }

    static TimePoint now()
    {
        return {std::chrono::system_clock::now()};
    }

    uint64_t milliseconds() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(point.time_since_epoch()).count();
    }

    uint64_t microseconds() const
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(point.time_since_epoch()).count();
    }

    float seconds() const
    {
        return std::chrono::duration_cast<std::chrono::duration<float>>(point.time_since_epoch()).count();
    }

    bool operator==(const TimePoint &t_other) const
    {
        return point == t_other.point;
    }

    bool operator!=(const TimePoint &t_other) const
    {
        return point != t_other.point;
    }

    bool operator<(const TimePoint &t_other) const
    {
        return point < t_other.point;
    }

    bool operator>(const TimePoint &t_other) const
    {
        return point > t_other.point;
    }

    bool operator<=(const TimePoint &t_other) const
    {
        return point <= t_other.point;
    }

    bool operator>=(const TimePoint &t_other) const
    {
        return point >= t_other.point;
    }

    Duration operator-(const TimePoint t_other) const
    {
        return {point - t_other.point};
    }

    TimePoint operator+(const Duration t_duration) const
    {
        return {point + t_duration.duration};
    }

    TimePoint operator-(const Duration t_duration) const
    {
        return {point - t_duration.duration};
    }

    TimePoint &operator-=(const Duration &t_duration)
    {
        point -= t_duration.duration;
        return *this;
    }

    TimePoint &operator+=(const Duration &t_duration)
    {
        point += t_duration.duration;
        return *this;
    }

    std::chrono::system_clock::time_point point = {};
};

inline auto format_as(const TimePoint t_point)
{
    return t_point.point;
}

} // namespace Immortals::Common
