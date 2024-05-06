#include "renderer.h"

namespace Tyr::Gui
{
static constexpr float kTextOffsetX = 50.0f;
static constexpr float kTextOffsetY = 300.0f;

void Renderer::drawRobots(const google::protobuf::RepeatedPtrField<Protos::SSL_DetectionRobot> &data,
                          Common::TeamColor                                                     color)
{
    for (const auto &robot : data)
    {
        drawRobot(robot, color);
    }
}

void Renderer::drawRobot(const Protos::SSL_DetectionRobot &robot, Common::TeamColor color)
{
    auto lTextColor = color == Common::TeamColor::Yellow ? Common::Color::black() : Common::Color::white();
    auto lBaseColor = color == Common::TeamColor::Yellow ? Common::Color::yellow() : Common::Color::blue();

    auto position = Common::Vec2(robot.x(), robot.y());
    drawCircleSector(Common::Circle{position, robotRadius}, lBaseColor,
                     robotArcAngle - Common::Angle::fromRad(robot.orientation()),
                     -robotArcAngle - Common::Angle::fromRad(robot.orientation()), true);
    drawText(position + Common::Vec2(kTextOffsetX, kTextOffsetY), std::to_string(robot.robot_id()), 14, lTextColor);
}
} // namespace Tyr::Gui
