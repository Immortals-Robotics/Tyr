#pragma once

namespace Tyr::Soccer
{
class OneTouchDetector
{
public:
    int                hys    = 0;
    Robot             *rState = nullptr;
    Common::BallState *ball   = nullptr;
    int               *side   = nullptr;

    float BAR = 89.0f;

    bool isArriving(const float angleTol = 40, const float passAngleLimit = 80)
    {
        return isArriving(Common::Vec2(-(*side) * Common::field().width, 0), angleTol, passAngleLimit);
    }

    bool isArriving(const Common::Vec2 &target, const float angleTol, const float passAngleLimit)
    {
        const float         ballDistBeforeStop = (ball->velocity.lengthSquared()) / 500.0f;
        const Common::Angle angleWithTarget    = rState->state().position.angleWith(target);
        if ((std::fabs((ball->velocity.toAngle() -
                        ball->position.angleWith(rState->state().position + angleWithTarget.toUnitVec() * BAR))
                           .deg()) < angleTol) &&
            (std::fabs(
                 (Common::Angle::fromDeg(180) + ball->velocity.toAngle() - rState->state().position.angleWith(target))
                     .deg()) < passAngleLimit) &&
            (rState->state().position.distanceTo(ball->position) < ballDistBeforeStop))
        {
            hys = 10;
            return true;
        }
        else if ((hys > 0) && (ball->velocity.length() > 50))
        {
            hys--;
            return true;
        }

        else
        {
            hys = 0;
            return false;
        }
    }
};
} // namespace Tyr::Soccer
