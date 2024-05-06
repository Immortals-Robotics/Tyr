#include "debug.h"

namespace Tyr::Common
{
Debug::Debug(NetworkAddress t_address, const bool t_enabled) : m_address(std::move(t_address))
{
    m_enabled = t_enabled;
    if (m_enabled)
    {
        logWarning("Debugger is enabled");
    }

    m_udp = std::make_unique<UdpServer>();
}

void Debug::broadcast()
{
    if (!m_enabled)
    {
        m_wrapper.Clear();
        logError("Debug::broadcast(): Can't broadcast!");
        return;
    }

    // TODO: set proper timestamp
    m_wrapper.set_timestamp(m_last_sent_frame_id++);

    // TODO: check result
    m_udp->send(m_wrapper, m_address);

    m_wrapper.Clear();
}

void Debug::draw(const Vec2 t_pos, const Color t_color, const std::string_view t_layer)
{
    Protos::Immortals::Debug::Draw *const draw = m_wrapper.add_draw();

    draw->set_layer(t_layer.data(), t_layer.size());
    // TODO: fill source location
    t_color.fillProto(draw->mutable_color());

    Protos::Immortals::Point *const point = draw->mutable_point();
    t_pos.fillProto(point->mutable_pos());
}

void Debug::draw(const Line &t_line, const Color t_color, const std::string_view t_layer)
{
    Protos::Immortals::Debug::Draw *const draw = m_wrapper.add_draw();

    draw->set_layer(t_layer.data(), t_layer.size());
    // TODO: fill source location
    t_color.fillProto(draw->mutable_color());

    Protos::Immortals::Line *const line = draw->mutable_line();
    t_line.fillProto(line);
}

void Debug::draw(const LineSegment &t_line, const Color t_color, const std::string_view t_layer)
{
    Protos::Immortals::Debug::Draw *const draw = m_wrapper.add_draw();

    draw->set_layer(t_layer.data(), t_layer.size());
    // TODO: fill source location
    t_color.fillProto(draw->mutable_color());

    Protos::Immortals::LineSegment *const line = draw->mutable_line_segment();
    t_line.fillProto(line);
}

void Debug::draw(const Rect &t_rect, const Color t_color, const std::string_view t_layer)
{
    Protos::Immortals::Debug::Draw *const draw = m_wrapper.add_draw();

    draw->set_layer(t_layer.data(), t_layer.size());
    // TODO: fill source location
    t_color.fillProto(draw->mutable_color());

    Protos::Immortals::Rect *const rect = draw->mutable_rect();
    t_rect.fillProto(rect);
}

void Debug::draw(const Circle &t_circle, const Color t_color, const std::string_view t_layer)
{
    Protos::Immortals::Debug::Draw *const draw = m_wrapper.add_draw();

    draw->set_layer(t_layer.data(), t_layer.size());
    // TODO: fill source location
    t_color.fillProto(draw->mutable_color());

    Protos::Immortals::Circle *const circle = draw->mutable_circle();
    t_circle.fillProto(circle);
}

void Debug::draw(const Triangle &t_triangle, const Color t_color, const std::string_view t_layer)
{
    Protos::Immortals::Debug::Draw *const draw = m_wrapper.add_draw();

    draw->set_layer(t_layer.data(), t_layer.size());
    // TODO: fill source location
    t_color.fillProto(draw->mutable_color());

    Protos::Immortals::Triangle *const triangle = draw->mutable_triangle();
    t_triangle.fillProto(triangle);
}

void Debug::draw(const RobotState &t_robot, const Color t_color, const std::string_view t_layer)
{
    // TODO:
    Protos::Immortals::RobotState *const robot = nullptr;
    t_robot.fillProto(robot);
}

void Debug::draw(const BallState &t_ball, const Color t_color, const std::string_view t_layer)
{
    // TODO:
    Protos::Immortals::BallState *const ball = nullptr;
    t_ball.fillProto(ball);
}

void Debug::log(const std::string_view t_text)
{
    Protos::Immortals::Debug::Log *const log = m_wrapper.add_log();
    log->set_payload(t_text.data(), t_text.size());
}
} // namespace Tyr::Common
