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
    auto lTextColor = color == Common::TeamColor::Yellow ? BLACK : WHITE;
    auto lBaseColor = color == Common::TeamColor::Yellow ? YELLOW : BLUE;

    auto position = Common::Vec2(robot.x(), robot.y());
    drawCircleSector({position, this->robotRadius}, lBaseColor, this->robotArcAngle - robot.orientation() * RAD2DEG,
                     360. - this->robotArcAngle - robot.orientation() * RAD2DEG, true);
    drawText(position + Common::Vec2(kTextOffsetX, kTextOffsetY), std::to_string(robot.robot_id()), 14, lTextColor);
}
} // namespace Tyr::Gui
