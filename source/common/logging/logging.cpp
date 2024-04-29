#include "logging.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "../services.h"

namespace Tyr::Common
{
Logger::Logger()
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::warn);

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/multisink.txt", true);
    file_sink->set_level(spdlog::level::trace);

    m_logger = std::make_unique<spdlog::logger>("default", spdlog::sinks_init_list{console_sink, file_sink});
    m_logger->set_level(spdlog::level::debug);
}
} // namespace Tyr::Common
