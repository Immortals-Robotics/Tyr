#include "logging.h"

namespace Tyr::Common
{
Logger::Logger()
{
    quill::Config config;
    config.enable_console_colours = true;

    {
        const std::filesystem::path log_dir(LOG_DIR);
        const std::filesystem::path log_file_name("log.txt");

        const std::filesystem::path log_path = log_dir / log_file_name;

        quill::FileHandlerConfig file_config;
        file_config.set_open_mode('w');
        file_config.set_append_to_filename(quill::FilenameAppend::StartDateTime);

        std::shared_ptr<quill::Handler> file_handler = quill::file_handler(log_path, file_config);
        config.default_handlers.push_back(std::move(file_handler));
    }

    quill::configure(config);
    quill::start();

    m_logger = quill::get_root_logger();
    m_logger->set_log_level(quill::LogLevel::TraceL3);

    LOG_INFO("Initialized the logger");
}
} // namespace Tyr::Common
