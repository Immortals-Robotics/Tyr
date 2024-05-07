#pragma once

#include "../math/vector.h"
#include "../network/udp_server.h"
#include "../state/world.h"
#include "color.h"

namespace Tyr::Common
{
class Debug
{
private:
    Debug(NetworkAddress t_address, bool t_enabled);
    ~Debug() = default;

    friend struct Services;

public:
    Debug(const Debug &)            = delete;
    Debug &operator=(const Debug &) = delete;

    void broadcast();

    void draw(Vec2 t_pos, Color t_color = Color::white());
    void draw(const Line &t_line, Color t_color = Color::white(), float t_thickness = 1.0f);
    void draw(const LineSegment &t_line, Color t_color = Color::white(), float t_thickness = 1.0f);
    void draw(const Rect &t_rect, Color t_color = Color::white(), bool t_filled = true, float t_thickness = 1.0f);
    void draw(const Circle &t_circle, Color t_color = Color::white(), bool t_filled = true, float t_thickness = 1.0f);
    void draw(const Triangle &t_triangle, Color t_color = Color::white(), bool t_filled = true,
              float t_thickness = 1.0f);

    void draw(const RobotState &t_robot, Color t_color = Color::white());
    void draw(const BallState &t_ball, Color t_color = Color::white());

    // Text Logging
    void log(std::string_view t_text);

protected:
    bool m_enabled;

private:
    static inline Debug *m_instance;

    // UDP_connection
    NetworkAddress             m_address;
    std::unique_ptr<UdpServer> m_udp;

    Protos::Immortals::Debug::Wrapper m_wrapper;

    uint32_t m_last_sent_frame_id = 0;
};
} // namespace Tyr::Common
