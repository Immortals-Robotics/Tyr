#pragma once
#include "../time/duration.h"

namespace Immortals::Common::Debug
{
struct ExecutionTime
{
    Duration interval;
    Duration duration;

    ExecutionTime() = default;

    explicit ExecutionTime(const Protos::Immortals::Debug::ExecutionTime &t_time)
    {
        interval = Duration::fromMicroseconds(t_time.interval());
        duration = Duration::fromMicroseconds(t_time.duration());
    }

    void fillProto(Protos::Immortals::Debug::ExecutionTime *t_time) const
    {
        t_time->set_interval(interval.microseconds());
        t_time->set_duration(duration.microseconds());
    }
};
} // namespace Immortals::Common::Debug
