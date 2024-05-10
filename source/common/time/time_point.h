#pragma once

namespace Tyr::Common
{
using TimeStamp = uint64_t;

struct TimePoint
{
    TimePoint() = default;

    TimePoint(const std::chrono::system_clock::time_point t_point) : point(t_point)
    {}

    TimePoint(const TimeStamp t_timestamp)
        : point(std::chrono::system_clock::time_point(std::chrono::milliseconds{t_timestamp}))
    {}

    static TimePoint now()
    {
        return TimePoint(std::chrono::system_clock::now());
    }

    TimeStamp timestamp() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(point.time_since_epoch()).count();
    }

    std::chrono::system_clock::time_point point;
};
} // namespace Tyr::Common
