#pragma once

namespace Tyr::Soccer
{
struct OpenAngle
{
    // Center of the angle
    Common::Angle center;
    // how big it is
    Common::Angle magnitude;

    static OpenAngle calculateOpenAngleToGoal(Common::Vec2 t_pos, const Robot& t_robot);
};


} // namespace Tyr::Soccer
