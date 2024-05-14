#pragma once

#include "../network/nng_server.h"
#include "../state/world.h"
#include "../time/time_point.h"
#include "color.h"

namespace Tyr::Common
{
class Debug
{
public:
    using StringMap = std::unordered_map<XXH32_hash_t, std::string>;

    struct SourceLocation
    {
        std::string_view file;
        std::string_view function;
        int              line;

        SourceLocation() = default;

        SourceLocation(const std::source_location &t_source);
        SourceLocation(const spdlog::source_loc &t_source);
        SourceLocation(const Protos::Immortals::Debug::SourceLocation &t_source, const StringMap &t_strings);

        void fillProto(Protos::Immortals::Debug::SourceLocation *t_source, StringMap *t_strings) const;
    };

    struct Draw
    {
        SourceLocation source;

        Color color     = Color::white();
        bool  filled    = true;
        float thickness = 1.0f;

        std::variant<Vec2, Line, LineSegment, Rect, Circle, Triangle> shape;

        Draw() = default;

        Draw(const Protos::Immortals::Debug::Draw &t_draw, const StringMap &t_strings);

        void fillProto(Protos::Immortals::Debug::Draw *t_draw, StringMap *t_strings) const;
    };

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

        Log(const Protos::Immortals::Debug::Log &t_log, const StringMap &t_strings);
        Log(const spdlog::details::log_msg &t_msg);

        void fillProto(Protos::Immortals::Debug::Log *t_log, StringMap *t_strings) const;

        Color color() const;

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

    struct Wrapper
    {
        TimePoint time;

        std::vector<Draw> draws;
        std::vector<Log>  logs;

        StringMap strings;

        Wrapper() = default;

        Wrapper(const Protos::Immortals::Debug::Wrapper &t_wrapper);

        void fillProto(Protos::Immortals::Debug::Wrapper *t_wrapper);
    };

    Debug(const Debug &)            = delete;
    Debug &operator=(const Debug &) = delete;

    void flush();

    void draw(Vec2 t_pos, Color t_color = Color::white(),
              std::source_location source = std::source_location::current());
    void draw(const Line &t_line, Color t_color = Color::white(), float t_thickness = 1.0f,
              std::source_location source = std::source_location::current());
    void draw(const LineSegment &t_line, Color t_color = Color::white(), float t_thickness = 1.0f,
              std::source_location source = std::source_location::current());
    void draw(const Rect &t_rect, Color t_color = Color::white(), bool t_filled = true, float t_thickness = 1.0f,
              std::source_location source = std::source_location::current());
    void draw(const Circle &t_circle, Color t_color = Color::white(), bool t_filled = true, float t_thickness = 1.0f,
              std::source_location source = std::source_location::current());
    void draw(const Triangle &t_triangle, Color t_color = Color::white(), bool t_filled = true,
              float t_thickness = 1.0f, std::source_location source = std::source_location::current());

    void log(Log &&t_log);
    void draw(Draw &&t_draw);

private:
    Debug();
    ~Debug() = default;

    friend struct Services;

    std::unique_ptr<NngServer> m_server;

    Wrapper m_wrapper;

    std::mutex m_log_mutex;
    std::mutex m_draw_mutex;
};
} // namespace Tyr::Common
