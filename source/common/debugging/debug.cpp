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

    m_wrapper = std::make_unique<Protos::Immortals::Imm_DBG_wrapper>();
}

void Debug::broadcast()
{
    if (!m_enabled)
    {
        m_wrapper->Clear();
        logError("Debug::broadcast(): Can't broadcast!");
        return;
    }

    m_wrapper->set_frame_id(m_last_sent_frame_id++);

    // TODO: check result
    m_udp->send(*m_wrapper, m_address);

    m_wrapper->Clear();
}

// TODO implement the files to send the points in their specific function_2D wrap
void Debug::plotPoint(const std::string_view t_func_name, const double t_x, const double t_y,
                      std::string_view t_point_name) const
{
    Protos::Immortals::point_2D *const point = m_wrapper->mutable_dbg_raw_points()->add_allpoints();

    point->set_name(t_func_name.data(), t_func_name.size());
    point->set_x(t_x);
    point->set_y(t_y);
    // point->set_name(t_point_name.data(), t_point_name.size());
}

void Debug::drawPoint(const Vec2 t_pos, const std::string_view t_layer, const Color t_color) const
{
    Protos::Immortals::Debug_Point *const point = m_wrapper->mutable_dbg_draw()->add_point();

    point->set_x(t_pos.x);
    point->set_y(t_pos.y);
    point->set_layer(t_layer.data(), t_layer.size());

    point->set_col_r(t_color.r);
    point->set_col_g(t_color.g);
    point->set_col_b(t_color.b);
}

void Debug::drawLineSegment(Vec2 t_pos1, Vec2 t_pos2, const std::string_view t_layer, const Color t_color) const
{
    Protos::Immortals::Debug_Line *const line = m_wrapper->mutable_dbg_draw()->add_line();

    line->set_x1(t_pos1.x);
    line->set_y1(t_pos1.y);
    line->set_x2(t_pos2.x);
    line->set_y2(t_pos2.y);
    line->set_layer(t_layer.data(), t_layer.size());

    line->set_col_r(t_color.r);
    line->set_col_g(t_color.g);
    line->set_col_b(t_color.b);
}

void Debug::drawRect(const Vec2 &p, float w, float h, const std::string_view t_layer, const Color t_color) const
{
    Protos::Immortals::Debug_Rect *const rect = m_wrapper->mutable_dbg_draw()->add_rect();

    rect->set_x1(p.x);
    rect->set_y1(p.y);
    rect->set_x2(p.x + w);
    rect->set_y2(p.y + h);
    rect->set_layer(t_layer.data(), t_layer.size());

    rect->set_col_r(t_color.r);
    rect->set_col_g(t_color.g);
    rect->set_col_b(t_color.b);
}

void Debug::drawCircle(const Vec2 &center, float r, const std::string_view t_layer, const Color t_color) const
{
    if (std::isnan(center.x) || std::isnan(center.y))
        return;

    Protos::Immortals::Debug_Circle *const circle = m_wrapper->mutable_dbg_draw()->add_circle();

    circle->set_x(center.x);
    circle->set_y(center.y);
    circle->set_r(r);
    circle->set_layer(t_layer.data(), t_layer.size());

    circle->set_col_r(t_color.r);
    circle->set_col_g(t_color.g);
    circle->set_col_b(t_color.b);
}

void Debug::drawRobot(const Vec2 t_pos, float t_angle, const int t_id, const std::string_view t_layer,
                      const Color t_color) const
{
    Protos::Immortals::Debug_Robot *const robot = m_wrapper->mutable_dbg_draw()->add_robot();

    robot->set_x(t_pos.x);
    robot->set_y(t_pos.y);
    robot->set_ang(t_angle);
    robot->set_id(t_id);
    robot->set_layer(t_layer.data(), t_layer.size());

    robot->set_col_r(t_color.r);
    robot->set_col_g(t_color.g);
    robot->set_col_b(t_color.b);
}

void Debug::logMessage(const std::string_view t_title, const std::string_view t_text) const
{
    Protos::Immortals::Debug_Text *log = m_wrapper->mutable_dbg_text()->add_log();

    log->set_title(t_title.data(), t_title.size());
    log->set_text(t_text.data(), t_text.size());
}

void Debug::logMessage(const std::string_view t_title, const double t_num) const
{
    logMessage(t_title, std::to_string(t_num));
}
} // namespace Tyr::Common
