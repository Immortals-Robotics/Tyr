#pragma once

namespace Tyr::Soccer
{
class OneTouchDetector
{
public:
    float               field_w;
    float               field_h;
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
        return IsArriving(Common::vec2(-(*side) * field_w, 0), angleTol, passAngleLimit);
    }
    bool IsArriving(const Common::vec2 &target, float angleTol, float passAngleLimit)
    {
        const float ballDistBeforeStop = (Common::length(bState->velocity) * Common::length(bState->velocity)) / 500.0f;
        // ballDistBeforeStop += 100.0f;
        const Common::Angle angleWithTarget = Common::angle_with(rState->Position, target);
        if ((fabs((Common::to_angle(bState->velocity) -
                   Common::angle_with(bState->Position, Common::vec2(rState->Position.x + BAR * angleWithTarget.cos(),
                                                                     rState->Position.y + BAR * angleWithTarget.sin())))
                      .deg()) < angleTol) &&
            (fabs((Common::Angle::fromDeg(180) + Common::to_angle(bState->velocity) -
                   Common::angle_with(rState->Position, target))
                      .deg()) < passAngleLimit) &&
            //( bState->velocity.magnitude > 50 ) )
            (Common::distance(rState->Position, bState->Position) < ballDistBeforeStop))
        //&&(abs(ball.vel_angle-90)>0.01)&&
        //(abs(ball.vel_angle+90)>0.01)&&
        //(abs(ball.vel_angle-180)>0.01)&&
        //(abs(ball.vel_angle+180)>0.01))
        {
            hys = 10;
            return true;
        }
        else if ((hys > 0) && (Common::length(bState->velocity) > 50))
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
