#pragma once

#define COLOR_BLUE false
#define COLOR_YELLOW true

#define RIGHT_SIDE true
#define LEFT_SIDE false

#include "../math/angle.h"
#include "../math/vector.h"

namespace Tyr::Common
{
enum SeenState
{
    Seen,
    CompletelyOut,
    TemprolilyOut
};

struct RobotState
{
    int vision_id;

    Vec2 Position;
    Vec2 velocity;

    Angle angle;
    Angle AngularVelocity;

    SeenState seenState;
    bool      OutForSubsitute;

    friend std::ostream &operator<<(std::ostream &oo, const RobotState &state)
    {
        oo << "Robot " << state.vision_id << "		X : " << (int) state.Position.x
           << "		Y : " << (int) state.Position.y << "		Tetta : " << (int) state.angle.deg() << std::endl;
        return oo;
    }
};

struct BallState
{
    Vec2 Position;
    Vec2 velocity;

    SeenState seenState;

    Vec2   First_Pos_when_shooted;
    Vec2   path_dir;
    double t_capture;

    friend std::ostream &operator<<(std::ostream &oo, const BallState &state)
    {
        oo << "Ball " << state.seenState << "		X : " << (int) state.Position.x
           << "		Y : " << (int) state.Position.y << std::endl;
        return oo;
    }
};

struct WorldState
{
    int  ownRobots_num, oppRobots_num;
    bool has_ball;

    BallState ball;

    RobotState OwnRobot[Setting::kMaxRobots];
    RobotState OppRobot[Setting::kMaxRobots];

    Vec3 lastCMDS[Setting::kMaxRobots][11];

    double t_capture       = -1;
    double delta_t_capture = -1;

    WorldState()
    {
        ball.Position  = Vec2();
        ball.velocity  = Vec2();
        ball.t_capture = 0.0f;
        ball.seenState = CompletelyOut;
        has_ball       = false;

        ownRobots_num = 0;
        for (int i = 0; i < Setting::kMaxRobots; i++)
        {
            OwnRobot[i].angle           = Angle();
            OwnRobot[i].AngularVelocity = Angle();
            OwnRobot[i].Position        = Vec2();
            OwnRobot[i].seenState       = CompletelyOut;
            OwnRobot[i].OutForSubsitute = true;
            OwnRobot[i].velocity        = Vec2();
            OwnRobot[i].vision_id       = i;
            for (int j = 0; j < 11; j++)
            {
                lastCMDS[i][j] = Vec3();
            }
        }
        oppRobots_num = 0;
        for (int i = 0; i < Setting::kMaxRobots; i++)
        { // TODO not in the last code...
            OppRobot[i].angle           = Angle();
            OppRobot[i].AngularVelocity = Angle();
            OppRobot[i].Position        = Vec2();
            OppRobot[i].seenState       = CompletelyOut;
            OppRobot[i].OutForSubsitute = true;
            OppRobot[i].velocity        = Vec2();
            OppRobot[i].vision_id       = i;
        }
    }

    friend std::ostream &operator<<(std::ostream &oo, const WorldState &state)
    {
        oo << (int) state.has_ball << " balls,	" << state.ownRobots_num << " Own Robots,	" << state.oppRobots_num
           << " Opp Robots." << std::endl;
        oo << (int) state.ball.Position.x << "	" << (int) state.ball.Position.y << std::endl;

        for (int i = 0; i < Setting::kMaxRobots; i++)
            if (state.OwnRobot[i].seenState != CompletelyOut)
                oo << state.OwnRobot[i];
        return oo;
    }
};
} // namespace Tyr::Common
