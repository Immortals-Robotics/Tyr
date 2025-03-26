#include "logging.h"

namespace Tyr::Common
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
    spdlog::init_thread_pool(8192, 4);
    m_logger = std::make_shared<spdlog::async_logger>("default", spdlog::sinks_init_list{}, spdlog::thread_pool(),
                                                      spdlog::async_overflow_policy::block);
    m_logger->set_level(spdlog::level::trace);
    m_logger->flush_on(spdlog::level::err);

    spdlog::register_logger(m_logger);
}

Logger::~Logger()
{
    flush();
}

void Logger::addDebugSink()
{
    auto debug_sink = std::make_shared<DebugSinkMt>();
    debug_sink->set_level(spdlog::level::trace);

    setPattern(debug_sink.get());

    m_logger->sinks().push_back(debug_sink);
}
} // namespace Tyr::Common
