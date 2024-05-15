#include "renderer.h"

namespace Tyr::Gui
{
void Renderer::draw(const Common::RawRobotState &t_robot)
{
    auto text_color = t_robot.color == Common::TeamColor::Yellow ? Common::Color::black() : Common::Color::white();
    auto base_color = t_robot.color == Common::TeamColor::Yellow ? Common::Color::yellow() : Common::Color::blue();

    drawCircleSector(Common::Circle{t_robot.position, Common::field().robot_radius}, base_color,
                     kRobotArcAngle - t_robot.angle.deg(), 360. - kRobotArcAngle - t_robot.angle.deg(), true);

    draw(t_robot.position, std::to_string(t_robot.vision_id), text_color, 180);
}

void Renderer::draw(const Common::RobotState &t_robot)
{
    if (t_robot.seen_state == Common::SeenState::CompletelyOut)
        return;

    auto text_color    = t_robot.color == Common::TeamColor::Yellow ? Common::Color::black() : Common::Color::white();
    auto base_color    = t_robot.color == Common::TeamColor::Yellow ? Common::Color::yellow() : Common::Color::blue();
    auto outline_color = Common::Color::black();

    if (t_robot.seen_state == Common::SeenState::TemprolilyOut)
    {
        base_color    = base_color.transparent();
        text_color    = text_color.transparent();
        outline_color = outline_color.transparent();
    }

    drawCircleSector(Common::Circle{t_robot.position, Common::field().robot_radius + 5.0f}, outline_color,
                     kRobotArcAngle - t_robot.angle.deg(), 360. - kRobotArcAngle - t_robot.angle.deg(), false);

    drawCircleSector(Common::Circle{t_robot.position, Common::field().robot_radius}, base_color,
                     kRobotArcAngle - t_robot.angle.deg(), 360. - kRobotArcAngle - t_robot.angle.deg(), true);

    draw(Common::LineSegment{t_robot.position, t_robot.position + t_robot.velocity / 2.0f}, outline_color.transparent(),
         1.0f);

    draw(t_robot.position, std::to_string(t_robot.vision_id), text_color, 180);
}
} // namespace Tyr::Gui
