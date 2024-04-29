#include "logging.h"

namespace Tyr::Common
{
Logger::Logger()
{
    const std::filesystem::path log_dir(LOG_DIR);
    const std::filesystem::path log_file_name("log.txt");

    const std::filesystem::path log_path = log_dir / log_file_name;

    std::shared_ptr<quill::Handler> file_handler =
        quill::file_handler(log_path,
                            []()
                            {
                                quill::FileHandlerConfig config;
                                config.set_open_mode('w');
                                config.set_append_to_filename(quill::FilenameAppend::StartDateTime);
                                return config;
                            }());

    quill::Config config;
    config.enable_console_colours = true;

    config.default_handlers.push_back(std::move(file_handler));

    quill::configure(config);

    quill::get_root_logger()->set_log_level(quill::LogLevel::TraceL3);

    quill::start();

    LOG_INFO("Initialized the logger");
}
} // namespace Tyr::Common
