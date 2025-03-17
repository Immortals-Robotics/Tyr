#pragma once

namespace Immortals::Common
{
class Logger
{
public:
    template <typename... Args>
    void log(const std::source_location source, spdlog::level::level_enum level,
             spdlog::format_string_t<Args...> format, Args &&...args)
    {
        spdlog::source_loc source_loc{source.file_name(), static_cast<int>(source.line()), source.function_name()};
        m_logger->log(source_loc, level, format, std::forward<Args>(args)...);
    }

    void flush();

protected:
    Logger();
    ~Logger();

    void addDebugSink();

    friend struct Services;

private:
    std::shared_ptr<spdlog::logger> m_logger;
};
} // namespace Immortals::Common
