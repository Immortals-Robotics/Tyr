#include "renderer.h"

namespace Tyr::Gui
{
static constexpr float kTextOffsetX = 50.0f;
static constexpr float kTextOffsetY = 300.0f;

void Renderer::draw(const Common::RawRobotState &t_robot)
{
    auto text_color = t_robot.color == Common::TeamColor::Yellow ? Common::Color::black() : Common::Color::white();
    auto base_color = t_robot.color == Common::TeamColor::Yellow ? Common::Color::yellow() : Common::Color::blue();

    drawCircleSector(Common::Circle{t_robot.position, Common::field().robot_radius}, base_color,
                     this->robotArcAngle - t_robot.angle.deg(), 360. - this->robotArcAngle - t_robot.angle.deg(), true);

    drawText(t_robot.position + Common::Vec2(kTextOffsetX, kTextOffsetY), std::to_string(t_robot.vision_id), 14,
             text_color);
}

void Renderer::draw(const Common::RobotState &t_robot)
{
    if (t_robot.seen_state == Common::SeenState::CompletelyOut)
        return;

    auto text_color = t_robot.color == Common::TeamColor::Yellow ? Common::Color::black() : Common::Color::white();
    auto base_color = t_robot.color == Common::TeamColor::Yellow ? Common::Color::yellow() : Common::Color::blue();

    if (t_robot.seen_state == Common::SeenState::TemprolilyOut)
    {
        base_color = base_color.transparent();
        text_color = text_color.transparent();
    }

    drawCircleSector(Common::Circle{t_robot.position, Common::field().robot_radius}, base_color,
                     this->robotArcAngle - t_robot.angle.deg(), 360. - this->robotArcAngle - t_robot.angle.deg(), true);

    drawText(t_robot.position + Common::Vec2(kTextOffsetX, kTextOffsetY), std::to_string(t_robot.vision_id), 14,
             text_color);
}
} // namespace Tyr::Gui
