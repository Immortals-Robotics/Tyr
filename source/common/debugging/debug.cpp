#include "debug.h"

namespace Tyr::Common
{
Debug::SourceLocation::SourceLocation(const std::source_location &t_source)
{
    file     = t_source.file_name();
    function = t_source.function_name();
    line     = t_source.line();
}

Debug::SourceLocation::SourceLocation(const spdlog::source_loc &t_source)
{
    file     = t_source.filename;
    function = t_source.funcname;
    line     = t_source.line;
}

Debug::SourceLocation::SourceLocation(const Protos::Immortals::Debug::SourceLocation &t_source,
                                      const StringMap                                &t_strings)
{
    file     = t_strings.at(t_source.file());
    function = t_strings.at(t_source.function());
    line     = t_source.line();
}

void Debug::SourceLocation::fillProto(Protos::Immortals::Debug::SourceLocation *const t_source,
                                      StringMap *const                                t_strings) const
{
    const XXH32_hash_t file_hash     = XXH32(file.data(), file.size(), 0);
    const XXH32_hash_t function_hash = XXH32(function.data(), function.size(), 0);

    t_source->set_file(file_hash);
    t_source->set_function(function_hash);
    t_source->set_line(line);

    if (!t_strings->contains(file_hash))
        t_strings->try_emplace(file_hash, std::string{file.data(), file.size()});
    if (!t_strings->contains(function_hash))
        t_strings->emplace(function_hash, std::string{function.data(), function.size()});
}

Debug::Draw::Draw(const Protos::Immortals::Debug::Draw &t_draw, const StringMap &t_strings)
{
    source    = SourceLocation{t_draw.source(), t_strings};
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
        logWarning("Unsupported shape type: {}", (int) t_draw.shape_case());
        break;
    }
}

void Debug::Draw::fillProto(Protos::Immortals::Debug::Draw *t_draw, StringMap *t_strings) const
{
    source.fillProto(t_draw->mutable_source(), t_strings);
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
        logWarning("Unsupported shape type: {}", shape.index());
}

Debug::Log::Log(const Protos::Immortals::Debug::Log &t_log, const StringMap &t_strings)
{
    level  = (Level) t_log.level();
    source = SourceLocation{t_log.source(), t_strings};
    text   = t_log.text();
}

Debug::Log::Log(const spdlog::details::log_msg &t_msg)
{
    level  = (Level) t_msg.level;
    source = t_msg.source;
    text   = {t_msg.payload.data(), t_msg.payload.size()};
}

void Debug::Log::fillProto(Protos::Immortals::Debug::Log *t_log, StringMap *t_strings) const
{
    t_log->set_level((Protos::Immortals::Debug::Log_Level) level);
    source.fillProto(t_log->mutable_source(), t_strings);
    t_log->set_text(text);
}

Color Debug::Log::color() const
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

Debug::Wrapper::Wrapper(const Protos::Immortals::Debug::Wrapper &t_wrapper)
{
    time = t_wrapper.time();

    strings.insert(t_wrapper.strings().begin(), t_wrapper.strings().end());

    draws.reserve(t_wrapper.draw_size());
    logs.reserve(t_wrapper.log_size());

    for (const auto &draw : t_wrapper.draw())
        draws.emplace_back(draw, strings);

    for (const auto &log : t_wrapper.log())
        logs.emplace_back(log, strings);
}

void Debug::Wrapper::fillProto(Protos::Immortals::Debug::Wrapper *t_wrapper)
{
    t_wrapper->set_time(time.timestamp());

    for (const auto &draw : draws)
        draw.fillProto(t_wrapper->add_draw(), &strings);
    for (const auto &log : logs)
        log.fillProto(t_wrapper->add_log(), &strings);

    t_wrapper->mutable_strings()->insert(strings.begin(), strings.end());
}

Debug::Debug()
{
    m_server = std::make_unique<NngServer>(setting().debug_url);
}

void Debug::flush()
{
    logger().flush();

    m_log_mutex.lock();
    m_draw_mutex.lock();

    m_wrapper.time = TimePoint::now();

    Protos::Immortals::Debug::Wrapper pb_wrapper;
    m_wrapper.fillProto(&pb_wrapper);

    m_wrapper.draws.clear();
    m_wrapper.logs.clear();

    m_log_mutex.unlock();
    m_draw_mutex.unlock();

    m_server->send(m_wrapper.time, pb_wrapper);
}

void Debug::draw(const Vec2 t_pos, const Color t_color, const std::source_location source)
{
    Draw draw{};
    draw.source = source;
    draw.shape  = t_pos;
    draw.color  = t_color;

    this->draw(std::move(draw));
}

void Debug::draw(const Line &t_line, const Color t_color, const float t_thickness, const std::source_location source)
{
    Draw draw{};
    draw.source    = source;
    draw.shape     = t_line;
    draw.color     = t_color;
    draw.thickness = t_thickness;

    this->draw(std::move(draw));
}

void Debug::draw(const LineSegment &t_line, const Color t_color, const float t_thickness,
                 const std::source_location source)
{
    Draw draw{};
    draw.source    = source;
    draw.shape     = t_line;
    draw.color     = t_color;
    draw.thickness = t_thickness;

    this->draw(std::move(draw));
}

void Debug::draw(const Rect &t_rect, const Color t_color, const bool t_filled, const float t_thickness,
                 const std::source_location source)
{
    Draw draw{};
    draw.source    = source;
    draw.shape     = t_rect;
    draw.color     = t_color;
    draw.filled    = t_filled;
    draw.thickness = t_thickness;

    this->draw(std::move(draw));
}

void Debug::draw(const Circle &t_circle, const Color t_color, const bool t_filled, const float t_thickness,
                 const std::source_location source)
{
    Draw draw{};
    draw.source    = source;
    draw.shape     = t_circle;
    draw.color     = t_color;
    draw.filled    = t_filled;
    draw.thickness = t_thickness;

    this->draw(std::move(draw));
}

void Debug::draw(const Triangle &t_triangle, const Color t_color, const bool t_filled, const float t_thickness,
                 const std::source_location source)
{
    Draw draw{};
    draw.source    = source;
    draw.shape     = t_triangle;
    draw.color     = t_color;
    draw.filled    = t_filled;
    draw.thickness = t_thickness;

    this->draw(std::move(draw));
}

void Debug::log(Log &&t_log)
{
    m_log_mutex.lock();
    m_wrapper.logs.emplace_back(t_log);
    m_log_mutex.unlock();
}

void Debug::draw(Draw &&t_draw)
{
    m_draw_mutex.lock();
    m_wrapper.draws.emplace_back(t_draw);
    m_draw_mutex.unlock();
}
} // namespace Tyr::Common
