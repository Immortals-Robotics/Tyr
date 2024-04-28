#include "VisualizationRenderer.h"

#define TEXT_OFFSET_X 50.
#define TEXT_OFFSET_Y 300.
namespace Tyr::Gui
{
void VisualizationRenderer::DrawRobots(const google::protobuf::RepeatedPtrField<Protos::SSL_DetectionRobot> &data,
                                       Tyr::Common::TeamColor                                                color)
{
    auto lTextColor = color == Tyr::Common::TeamColor::Yellow ? BLACK : WHITE;
    auto lBaseColor = color == Tyr::Common::TeamColor::Yellow ? YELLOW : BLUE;
    for (auto robot : data)
    {
        auto position = ImVec2(robot.x(), robot.y());
        DrawCircleSectorVec(position, this->robotRadius, lBaseColor,
                            this->robotArcAngle - robot.orientation() * RAD2DEG,
                            360. - this->robotArcAngle - robot.orientation() * RAD2DEG, true);
        DrawTextVec(position + ImVec2(TEXT_OFFSET_X, TEXT_OFFSET_Y), std::to_string(robot.robot_id()), 14, lTextColor);
    }
}
} // namespace Tyr::Gui
