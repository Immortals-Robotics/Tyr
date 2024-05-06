#include "renderer.h"

namespace Tyr::Gui
{
static constexpr float kTextOffsetX = 50.0f;
static constexpr float kTextOffsetY = 300.0f;

void Renderer::draw(const Common::RobotState &t_robot)
{
    auto lTextColor = t_robot.color == Common::TeamColor::Yellow ? Common::Color::black() : Common::Color::white();
    auto lBaseColor = t_robot.color == Common::TeamColor::Yellow ? Common::Color::yellow() : Common::Color::blue();

    drawCircleSector(Common::Circle{t_robot.position, robotRadius}, lBaseColor, robotArcAngle - t_robot.angle,
                     -robotArcAngle - t_robot.angle, true);
    drawText(t_robot.position + Common::Vec2(kTextOffsetX, kTextOffsetY), std::to_string(t_robot.vision_id), 14,
             lTextColor);
}
} // namespace Tyr::Gui
