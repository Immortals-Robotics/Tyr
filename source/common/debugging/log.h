#pragma once

#include "color.h"
#include "source_location.h"

namespace Immortals::Common::Debug
{
struct Log
{
    enum class Level
    {
        Trace    = SPDLOG_LEVEL_TRACE,
        Debug    = SPDLOG_LEVEL_DEBUG,
        Info     = SPDLOG_LEVEL_INFO,
        Warning  = SPDLOG_LEVEL_WARN,
        Error    = SPDLOG_LEVEL_ERROR,
        Critical = SPDLOG_LEVEL_CRITICAL,
    };

    Level          level;
    SourceLocation source;

    std::string text;

    Log() = default;

    Log(const Protos::Immortals::Debug::Log &t_log, const StringMap &t_strings)
    {
        level  = static_cast<Level>(t_log.level());
        source = SourceLocation{t_log.source(), t_strings};
        text   = t_log.text();
    }

    explicit Log(const spdlog::details::log_msg &t_msg)
    {
        level  = static_cast<Level>(t_msg.level);
        source = SourceLocation{t_msg.source};
        text   = {t_msg.payload.data(), t_msg.payload.size()};
    }

    void fillProto(Protos::Immortals::Debug::Log *t_log, StringMap *t_strings) const
    {
        t_log->set_level(static_cast<Protos::Immortals::Debug::Log_Level>(level));
        source.fillProto(t_log->mutable_source(), t_strings);
        t_log->set_text(text);
    }

    Color color() const
    {
        switch (level)
        {
        case Level::Trace:
            return Color::gray();
        case Level::Debug:
            return Color::blue();
        case Level::Info:
            return Color::green();
        case Level::Warning:
            return Color::yellow();
        case Level::Error:
            return Color::orange();
        case Level::Critical:
            return Color::red();
        default:
            return Color::white();
        }
    }

    std::string_view levelName() const
    {
        switch (level)
        {
        case Level::Trace:
            return "Trace";
        case Level::Debug:
            return "Debug";
        case Level::Info:
            return "Info";
        case Level::Warning:
            return "Warning";
        case Level::Error:
            return "Error";
        case Level::Critical:
            return "Critical";
        default:
            return "Unknown";
        }
    }
};
} // namespace Immortals::Common::Debug
