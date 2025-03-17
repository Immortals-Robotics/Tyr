#include "logging.h"

namespace Immortals::Common
{
template <typename Sink>
static void setPattern(Sink *const t_sink)
{
    t_sink->set_pattern("%^[%a %H:%M:%S.%e] [%t] [%l] [%s:%#] %v%$");
}

void Logger::flush()
{
    m_logger->flush();
}

Logger::Logger()
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::trace);

    setPattern(console_sink.get());

    spdlog::init_thread_pool(8192, 1);
    m_logger = std::make_shared<spdlog::async_logger>("default", console_sink, spdlog::thread_pool(),
                                                      spdlog::async_overflow_policy::block);
    m_logger->set_level(spdlog::level::trace);
    m_logger->flush_on(spdlog::level::err);

    spdlog::register_logger(m_logger);
}

Logger::~Logger()
{
    flush();
}

#if FEATURE_DEBUG
void Logger::addDebugSink()
{
    auto debug_sink = std::make_shared<DebugSinkMt>();
    debug_sink->set_level(spdlog::level::trace);

    setPattern(debug_sink.get());

    m_logger->sinks().push_back(debug_sink);
}
#endif
} // namespace Immortals::Common
