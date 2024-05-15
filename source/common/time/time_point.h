#pragma once

namespace Tyr::Common
{
using TimeStampInt    = uint64_t;
using TimeStampDouble = double;

struct TimePoint
{
    inline TimePoint() = default;

    inline TimePoint(const std::chrono::system_clock::time_point t_point) : point(t_point)
    {}

    inline static TimePoint fromMilliseconds(const TimeStampInt t_timestamp)
    {
        return std::chrono::system_clock::time_point(std::chrono::milliseconds{t_timestamp});
    }

    inline static TimePoint fromMicroseconds(const TimeStampInt t_timestamp)
    {
        return std::chrono::system_clock::time_point(std::chrono::microseconds{t_timestamp});
    }

    inline static TimePoint fromSeconds(const TimeStampDouble t_timestamp)
    {
        const auto duration = std::chrono::duration<float>{t_timestamp};
        return std::chrono::system_clock::time_point(std::chrono::round<std::chrono::microseconds>(duration));
    }

    inline static TimePoint now()
    {
        return TimePoint(std::chrono::system_clock::now());
    }

    inline TimeStampInt milliseconds() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(point.time_since_epoch()).count();
    }

    inline TimeStampInt microseconds() const
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(point.time_since_epoch()).count();
    }

    inline TimeStampDouble seconds() const
    {
        return std::chrono::duration_cast<std::chrono::duration<TimeStampDouble>>(point.time_since_epoch()).count();
    }

    std::chrono::system_clock::time_point point;
};

inline auto format_as(TimePoint t_point)
{
    return t_point.point;
}

} // namespace Tyr::Common
