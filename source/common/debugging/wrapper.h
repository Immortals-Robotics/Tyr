#pragma once

#include "draw.h"
#include "log.h"

#include "../time/time_point.h"

namespace Tyr::Common::Debug
{
struct Wrapper
{
    TimePoint time;

    std::vector<Draw> draws;

    std::vector<Log> logs;

    StringMap strings;

    std::map<std::string, ExecutionTime> execution_times;

    Wrapper() = default;

    explicit Wrapper(const Protos::Immortals::Debug::Wrapper &t_wrapper)
    {
        time = TimePoint::fromMicroseconds(t_wrapper.time());

        strings.reserve(t_wrapper.strings().size());
        for (const auto &entry : t_wrapper.strings())
            strings.emplace(entry.first, entry.second);

        draws.reserve(t_wrapper.draw_size());

        logs.reserve(t_wrapper.log_size());

        for (const auto &draw : t_wrapper.draw())
            draws.emplace_back(draw, strings);

        for (const auto &log : t_wrapper.log())
            logs.emplace_back(log, strings);

        for (const auto &execution_time : t_wrapper.execution_times())
            execution_times.emplace(execution_time.first, execution_time.second);
    }

    void fillProto(Protos::Immortals::Debug::Wrapper *t_wrapper)
    {
        t_wrapper->set_time(time.microseconds());

        for (const auto &draw : draws)
            draw.fillProto(t_wrapper->add_draw(), &strings);

        for (const auto &log : logs)
            log.fillProto(t_wrapper->add_log(), &strings);

        for (const auto &entry : strings)
            t_wrapper->mutable_strings()->emplace(entry.first, entry.second);

        for (const auto &execution_time : execution_times)
            execution_time.second.fillProto(&(*t_wrapper->mutable_execution_times())[execution_time.first]);
    }
};
} // namespace Tyr::Common::Debug
