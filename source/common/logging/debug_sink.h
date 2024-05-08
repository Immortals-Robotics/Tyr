#pragma once

#include "../debugging/debug.h"
#include "../services.h"

namespace Tyr::Common
{
template <typename Mutex>
class DebugSink final : public spdlog::sinks::base_sink<Mutex>
{
protected:
    void sink_it_(const spdlog::details::log_msg &msg) override
    {
        spdlog::memory_buf_t formatted;
        spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);

        Debug::Log log{msg};
        log.text = fmt::to_string(formatted);

        debug().log(log);
    }
    void flush_() override
    {}
};

using DebugSinkMt = DebugSink<std::mutex>;
using DebugSinkSt = DebugSink<spdlog::details::null_mutex>;

} // namespace Tyr::Common
