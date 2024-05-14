#include "hub.h"

namespace Tyr::Common::Debug
{
Hub::Hub()
{
    m_server = std::make_unique<NngServer>(setting().debug_url);
}

void Hub::flush()
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

void Hub::draw(const Vec2 t_pos, const Color t_color, const std::source_location source)
{
    Draw draw{};
    draw.source = source;
    draw.shape  = t_pos;
    draw.color  = t_color;

    this->draw(std::move(draw));
}

void Hub::draw(const Line &t_line, const Color t_color, const float t_thickness, const std::source_location source)
{
    Draw draw{};
    draw.source    = source;
    draw.shape     = t_line;
    draw.color     = t_color;
    draw.thickness = t_thickness;

    this->draw(std::move(draw));
}

void Hub::draw(const LineSegment &t_line, const Color t_color, const float t_thickness,
               const std::source_location source)
{
    Draw draw{};
    draw.source    = source;
    draw.shape     = t_line;
    draw.color     = t_color;
    draw.thickness = t_thickness;

    this->draw(std::move(draw));
}

void Hub::draw(const Rect &t_rect, const Color t_color, const bool t_filled, const float t_thickness,
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

void Hub::draw(const Circle &t_circle, const Color t_color, const bool t_filled, const float t_thickness,
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

void Hub::draw(const Triangle &t_triangle, const Color t_color, const bool t_filled, const float t_thickness,
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

void Hub::log(Log &&t_log)
{
    m_log_mutex.lock();
    m_wrapper.logs.emplace_back(t_log);
    m_log_mutex.unlock();
}

void Hub::draw(Draw &&t_draw)
{
    m_draw_mutex.lock();
    m_wrapper.draws.emplace_back(t_draw);
    m_draw_mutex.unlock();
}
} // namespace Tyr::Common::Debug
