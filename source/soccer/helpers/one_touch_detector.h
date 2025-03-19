#pragma once

namespace Tyr::Soccer
{
class OneTouchDetector
{
private:
    static constexpr float kBar = 89.0f;

    int m_hys = 0;

public:
    const Common::RobotState *robot = nullptr;

    bool isArriving(const float angleTol = 40, const float passAngleLimit = 80)
    {
        return isArriving(Common::Vec2(-(State::side()) * Common::field().width, 0), angleTol, passAngleLimit);
    }

    bool isArriving(const Common::Vec2 &target, const float angleTol, const float passAngleLimit)
    {
        const float         ballDistBeforeStop = (State::world().ball.velocity.lengthSquared()) / 500.0f;
        const Common::Angle angleWithTarget    = robot->position.angleWith(target);
        if ((std::fabs(
                 (State::world().ball.velocity.toAngle() -
                  State::world().ball.position.angleWith(robot->position + angleWithTarget.toUnitVec() * kBar))
                     .deg()) < angleTol) &&
            (std::fabs((Common::Angle::fromDeg(180) + State::world().ball.velocity.toAngle() -
                        robot->position.angleWith(target))
                           .deg()) < passAngleLimit) &&
            (robot->position.distanceTo(State::world().ball.position) < ballDistBeforeStop))
        {
            m_hys = 10;
            return true;
        }
        else if ((m_hys > 0) && (State::world().ball.velocity.length() > 50))
        {
            m_hys--;
            return true;
        }
        else
        {
            m_hys = 0;
            return false;
        }
    }
};
} // namespace Tyr::Soccer
