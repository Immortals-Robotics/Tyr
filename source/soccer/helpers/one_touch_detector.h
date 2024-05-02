#pragma once

namespace Tyr::Soccer
{
class OneTouchDetector
{
public:
    int                 hys;
    Common::RobotState *rState;
    Common::BallState  *bState;
    int                *side;

    float BAR;

    OneTouchDetector()
    {
        hys = 0;
        BAR = 89.0;
    }
    bool IsArriving(float angleTol = 40, float passAngleLimit = 80)
    {
        return IsArriving(Common::Vec2(-(*side) * Common::worldState().field.width, 0), angleTol, passAngleLimit);
    }
    bool IsArriving(const Common::Vec2 &target, float angleTol, float passAngleLimit)
    {
        const float ballDistBeforeStop = (bState->velocity.lengthSquared()) / 500.0f;
        // ballDistBeforeStop += 100.0f;
        const Common::Angle angleWithTarget = rState->position.angleWith(target);
        if ((std::fabs((bState->velocity.toAngle() -
                   bState->position.angleWith(Common::Vec2(rState->position.x + BAR * angleWithTarget.cos(),
                                                           rState->position.y + BAR * angleWithTarget.sin())))
                      .deg()) < angleTol) &&
            (std::fabs((Common::Angle::fromDeg(180) + bState->velocity.toAngle() - rState->position.angleWith(target))
                      .deg()) < passAngleLimit) &&
            //( bState->velocity.length > 50 ) )
            (rState->position.distanceTo(bState->position) < ballDistBeforeStop))
        //&&(abs(ball.vel_angle-90)>0.01)&&
        //(abs(ball.vel_angle+90)>0.01)&&
        //(abs(ball.vel_angle-180)>0.01)&&
        //(abs(ball.vel_angle+180)>0.01))
        {
            hys = 10;
            return true;
        }
        else if ((hys > 0) && (bState->velocity.length() > 50))
        // &&(abs(ball.vel_angle-90)>0.01)&&(abs(ball.vel_angle+90)>0.01)&&(abs(ball.vel_angle-180)>0.01)&&(abs(ball.vel_angle+180)>0.01))
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
