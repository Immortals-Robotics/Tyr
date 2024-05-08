#include "logging.h"

#include "debug_sink.h"

namespace Tyr::Common
{
void Logger::flush()
{
    m_logger->flush();
}

Logger::Logger()
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::debug);

    auto debug_sink = std::make_shared<DebugSinkMt>();
    debug_sink->set_level(spdlog::level::debug);

    spdlog::init_thread_pool(8192, 1);
    m_logger = std::make_shared<spdlog::async_logger>("default", spdlog::sinks_init_list{console_sink, debug_sink},
                                                      spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    m_logger->set_level(spdlog::level::debug);
    spdlog::register_logger(m_logger);
}
} // namespace Tyr::Common
