#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::recievePass(int robot_num, Common::Vec2 staticPos, bool chip)
{
    const float contStrStaticTime = 1.0f;

    // oneTouchType[robot_num] = oneTouch;
    if (timer.time() > 0.7)
    {
        chip_head.setDeg(200);
    }
    if (oneTouchType[robot_num] == allaf)
        staticPos = allafPos[robot_num];
    if (oneTouchType[robot_num] == oneTouch && timer.time() < contStrStaticTime)
        staticPos = allafPos[robot_num];

    if (timer.time() > 2.5)
    {
        oneTouchTypeUsed[robot_num] = true;
    }

    float angleTol, maxBallAngle;

    if (oneTouchType[robot_num] == oneTouch)
    {
        angleTol     = 45;
        maxBallAngle = 150;
    }
    else if (oneTouchType[robot_num] == shirje)
    {
        // angleTol = 55;
        // maxBallAngle = 140;
        angleTol     = 45;
        maxBallAngle = 90;
    }
    else if (oneTouchType[robot_num] == gool)
    {
        angleTol     = 35;
        maxBallAngle = 110;
    }
    else
    { // probably allaf!!!
        angleTol     = 0;
        maxBallAngle = 0;
    }

    float distCoeff = Common::worldState().ball.position.distanceTo(OwnRobot[robot_num].state().position) / 1500.0f;
    distCoeff       = std::max(0.8f, distCoeff);
    distCoeff       = std::min(1.2f, distCoeff);

    if ((oneTouchDetector[robot_num].IsArriving(angleTol, maxBallAngle)) && (oneTouchType[robot_num] != allaf))
    {
        oneTouchTypeUsed[robot_num] = true;
        if (oneTouchType[robot_num] == oneTouch)
        {
            if (timer.time() < contStrStaticTime)
                WaitForPass(robot_num, chip, nullptr, &staticPos);
            else
                WaitForPass(robot_num, chip);
        }
        else if (oneTouchType[robot_num] == shirje)
        {
            WaitForOmghi(robot_num, chip);
            // WaitForPass(robot_num, 0, &OwnRobot[dmf].state().position);
        }
        else if (oneTouchType[robot_num] == gool)
        {
            WaitForGool(robot_num, chip);
        }
        else
        { // probably allaf!!!
            ERRTSetObstacles(robot_num, false, true);
            OwnRobot[robot_num].face(Common::Vec2(-side * Common::field().width, 0));
            // OwnRobot[robot_num].target.angle=-90;
            ERRTNavigate2Point(robot_num, staticPos, 100, &VELOCITY_PROFILE_MAMOOLI);
        }
    }
    else
    {
        if (oneTouchTypeUsed[robot_num])
        {
            oneTouchTypeUsed[robot_num] = false;
            oneTouchType[robot_num]     = oneTouch;
        }
        ERRTSetObstacles(robot_num, true, true);
        OwnRobot[robot_num].face(Common::Vec2(-side * Common::field().width, 0));
        // OwnRobot[robot_num].target.angle=-90;
        ERRTNavigate2Point(robot_num, staticPos, 100, &VELOCITY_PROFILE_MAMOOLI);
    }
}
} // namespace Tyr::Soccer
