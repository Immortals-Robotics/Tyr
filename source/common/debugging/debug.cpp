#include "debug.h"

namespace Tyr::Common
{
Debug::SourceLocation::SourceLocation(const std::source_location &t_source)
{
    file     = t_source.file_name();
    function = t_source.function_name();
    line     = t_source.line();
}

Debug::SourceLocation::SourceLocation(const Protos::Immortals::Debug::SourceLocation &t_source)
{
    file     = t_source.file();
    function = t_source.function();
    line     = t_source.line();
}

void Debug::SourceLocation::fillProto(Protos::Immortals::Debug::SourceLocation *t_source) const
{
    t_source->set_file(file.data(), file.size());
    t_source->set_function(function.data(), function.size());
    t_source->set_line(line);
}

Debug::Draw::Draw(const Protos::Immortals::Debug::Draw &t_draw)
{
    source    = t_draw.source();
    color     = t_draw.color();
    filled    = t_draw.filled();
    thickness = t_draw.thickness();

    switch (t_draw.shape_case())
    {
    case Protos::Immortals::Debug::Draw::kPoint:
        shape = Vec2{t_draw.point().pos()};
        break;
    case Protos::Immortals::Debug::Draw::kLine:
        shape = Line{t_draw.line()};
        break;
    case Protos::Immortals::Debug::Draw::kLineSegment:
        shape = LineSegment{t_draw.line_segment()};
        break;
    case Protos::Immortals::Debug::Draw::kRect:
        shape = Rect{t_draw.rect()};
        break;
    case Protos::Immortals::Debug::Draw::kCircle:
        shape = Circle{t_draw.circle()};
        break;
    case Protos::Immortals::Debug::Draw::kTriangle:
        shape = Triangle{t_draw.triangle()};
        break;
    default:
        Common::logWarning("Unsupported shape type: {}", (int) t_draw.shape_case());
        break;
    }
}

void Debug::Draw::fillProto(Protos::Immortals::Debug::Draw *t_draw) const
{
    source.fillProto(t_draw->mutable_source());
    color.fillProto(t_draw->mutable_color());
    t_draw->set_filled(filled);
    t_draw->set_thickness(thickness);

    if (auto point = std::get_if<Vec2>(&shape); point)
        point->fillProto(t_draw->mutable_point()->mutable_pos());
    else if (auto line = std::get_if<Line>(&shape); line)
        line->fillProto(t_draw->mutable_line());
    else if (auto line = std::get_if<LineSegment>(&shape); line)
        line->fillProto(t_draw->mutable_line_segment());
    else if (auto rect = std::get_if<Rect>(&shape); rect)
        rect->fillProto(t_draw->mutable_rect());
    else if (auto circle = std::get_if<Circle>(&shape); circle)
        circle->fillProto(t_draw->mutable_circle());
    else if (auto triangle = std::get_if<Triangle>(&shape); triangle)
        triangle->fillProto(t_draw->mutable_triangle());
    else
        Common::logWarning("Unsupported shape type: {}", shape.index());
}

Debug::Log::Log(const Protos::Immortals::Debug::Log &t_log)
{
    level       = (Level) t_log.level();
    source      = t_log.source();
    logger_name = t_log.logger();
    color       = t_log.color();
    payload     = t_log.payload();
}

void Debug::Log::fillProto(Protos::Immortals::Debug::Log *t_log) const
{
    t_log->set_level((Protos::Immortals::Debug::Log_Level) level);
    source.fillProto(t_log->mutable_source());
    t_log->set_logger(logger_name.data(), logger_name.size());
    color.fillProto(t_log->mutable_color());
    t_log->set_payload(payload);
}

Debug::Wrapper::Wrapper(const Protos::Immortals::Debug::Wrapper &t_wrapper)
{
    timestamp = t_wrapper.timestamp();

    draws = std::vector<Draw>{t_wrapper.draw().begin(), t_wrapper.draw().end()};
    logs  = std::vector<Log>{t_wrapper.log().begin(), t_wrapper.log().end()};
}

void Debug::Wrapper::fillProto(Protos::Immortals::Debug::Wrapper *t_wrapper) const
{
    t_wrapper->set_timestamp(timestamp);

    for (const auto &draw : draws)
        draw.fillProto(t_wrapper->add_draw());
    for (const auto &log : logs)
        log.fillProto(t_wrapper->add_log());
}

Debug::Debug(const bool t_enabled)
{
    m_enabled = t_enabled;
    if (m_enabled)
    {
        logWarning("Debugger is enabled");
    }
}

void Debug::flip()
{
    const auto now          = std::chrono::system_clock::now();
    m_wrapper_off.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    std::swap(m_wrapper, m_wrapper_off);

    m_wrapper_off.draws.clear();
    m_wrapper_off.logs.clear();
}

void Debug::draw(const Vec2 t_pos, const Color t_color)
{
    Draw draw{};
    draw.shape = t_pos;
    draw.color = t_color;

    m_wrapper_off.draws.push_back(draw);
}

void Debug::draw(const Line &t_line, const Color t_color, const float t_thickness)
{
    Draw draw{};
    draw.shape     = t_line;
    draw.color     = t_color;
    draw.thickness = t_thickness;

    m_wrapper_off.draws.push_back(draw);
}

void Debug::draw(const LineSegment &t_line, const Color t_color, const float t_thickness)
{
    Draw draw{};
    draw.shape     = t_line;
    draw.color     = t_color;
    draw.thickness = t_thickness;

    m_wrapper_off.draws.push_back(draw);
}

void Debug::draw(const Rect &t_rect, const Color t_color, const bool t_filled, const float t_thickness)
{
    Draw draw{};
    draw.shape     = t_rect;
    draw.color     = t_color;
    draw.filled    = t_filled;
    draw.thickness = t_thickness;

    m_wrapper_off.draws.push_back(draw);
}

void Debug::draw(const Circle &t_circle, const Color t_color, const bool t_filled, const float t_thickness)
{
    Draw draw{};
    draw.shape     = t_circle;
    draw.color     = t_color;
    draw.filled    = t_filled;
    draw.thickness = t_thickness;

    m_wrapper_off.draws.push_back(draw);
}

void Debug::draw(const Triangle &t_triangle, const Color t_color, const bool t_filled, const float t_thickness)
{
    Draw draw{};
    draw.shape     = t_triangle;
    draw.color     = t_color;
    draw.filled    = t_filled;
    draw.thickness = t_thickness;

    m_wrapper_off.draws.push_back(draw);
}

void Debug::log(const std::string_view t_text)
{
    Log log{};
    log.payload = t_text;

    m_wrapper_off.logs.push_back(log);
}
} // namespace Tyr::Common
