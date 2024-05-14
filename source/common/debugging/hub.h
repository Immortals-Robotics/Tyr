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
    Hub();
    ~Hub() = default;

    friend struct Services;

    std::unique_ptr<NngServer> m_server;

    Wrapper m_wrapper;

    std::mutex m_log_mutex;
    std::mutex m_draw_mutex;
};
} // namespace Tyr::Common::Debug
