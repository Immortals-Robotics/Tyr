#pragma once

#include "color.h"
#include "draw.h"
#include "log.h"
#include "source_location.h"
#include "wrapper.h"

#include "../network/nng_server.h"
#include "../state/world.h"
#include "../time/time_point.h"

namespace Tyr::Common::Debug
{
class Hub
{
public:
    Hub(const Hub &)            = delete;
    Hub &operator=(const Hub &) = delete;

    void flush()
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

    inline void draw(Vec2 t_pos, Color t_color = Color::white(),
                     std::source_location source = std::source_location::current())
    {
        Draw draw{};
        draw.source = source;
        draw.shape  = t_pos;
        draw.color  = t_color;

        this->draw(std::move(draw));
    }

    inline void draw(const Line &t_line, Color t_color = Color::white(), float t_thickness = 1.0f,
                     std::source_location source = std::source_location::current())
    {
        Draw draw{};
        draw.source    = source;
        draw.shape     = t_line;
        draw.color     = t_color;
        draw.thickness = t_thickness;

        this->draw(std::move(draw));
    }

    inline void draw(const LineSegment &t_line, Color t_color = Color::white(), float t_thickness = 1.0f,
                     std::source_location source = std::source_location::current())
    {
        Draw draw{};
        draw.source    = source;
        draw.shape     = t_line;
        draw.color     = t_color;
        draw.thickness = t_thickness;

        this->draw(std::move(draw));
    }

    inline void draw(const Rect &t_rect, Color t_color = Color::white(), bool t_filled = true, float t_thickness = 1.0f,
                     std::source_location source = std::source_location::current())
    {
        Draw draw{};
        draw.source    = source;
        draw.shape     = t_rect;
        draw.color     = t_color;
        draw.filled    = t_filled;
        draw.thickness = t_thickness;

        this->draw(std::move(draw));
    }

    inline void draw(const Circle &t_circle, Color t_color = Color::white(), bool t_filled = true,
                     float t_thickness = 1.0f, std::source_location source = std::source_location::current())
    {
        Draw draw{};
        draw.source    = source;
        draw.shape     = t_circle;
        draw.color     = t_color;
        draw.filled    = t_filled;
        draw.thickness = t_thickness;

        this->draw(std::move(draw));
    }

    inline void draw(const Triangle &t_triangle, Color t_color = Color::white(), bool t_filled = true,
                     float t_thickness = 1.0f, std::source_location source = std::source_location::current())
    {
        Draw draw{};
        draw.source    = source;
        draw.shape     = t_triangle;
        draw.color     = t_color;
        draw.filled    = t_filled;
        draw.thickness = t_thickness;

        this->draw(std::move(draw));
    }

    inline void log(Log &&t_log)
    {
        m_log_mutex.lock();
        m_wrapper.logs.emplace_back(t_log);
        m_log_mutex.unlock();
    }

    inline void draw(Draw &&t_draw)
    {
        m_draw_mutex.lock();
        m_wrapper.draws.emplace_back(t_draw);
        m_draw_mutex.unlock();
    }

private:
    inline Hub()
    {
        m_server = std::make_unique<NngServer>(setting().debug_url);
    }

    ~Hub() = default;

    friend struct ::Tyr::Common::Services;

    std::unique_ptr<NngServer> m_server;

    Wrapper m_wrapper;

    std::mutex m_log_mutex;
    std::mutex m_draw_mutex;
};
} // namespace Tyr::Common::Debug
