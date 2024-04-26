#pragma once

#define COLOR_BLUE false
#define COLOR_YELLOW true

#define RIGHT_SIDE true
#define LEFT_SIDE false

#include "../math/vector.h"

namespace Tyr::Common
{
enum SeenState
{
    Seen,
    CompletelyOut,
    TemprolilyOut
};

struct Velocity
{
    float x;
    float y;

    float magnitude;
    float direction;
};

struct RobotState
{
    int vision_id;

    vec2     Position;
    Velocity velocity;

    float Angle = 0.0f;
    float AngularVelocity;

    SeenState seenState;
    bool      OutForSubsitute;

    friend std::ostream &operator<<(std::ostream &oo, const RobotState &state)
    {
        oo << "Robot " << state.vision_id << "		X : " << (int) state.Position.x
           << "		Y : " << (int) state.Position.y << "		Tetta : " << (int) state.Angle << std::endl;
        return oo;
    }
};

struct BallState
{
    vec2     Position;
    Velocity velocity;

    SeenState seenState;

    vec2   First_Pos_when_shooted;
    vec2   path_dir;
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

    vec3 lastCMDS[Setting::kMaxRobots][11];

    double t_capture       = -1;
    double delta_t_capture = -1;

    WorldState()
    {
        ball.Position           = vec2(0.0f);
        ball.velocity.x         = 0.0f;
        ball.velocity.y         = 0.0f;
        ball.velocity.direction = 0.0f;
        ball.velocity.magnitude = 0.0f;
        ball.t_capture          = 0.0f;
        ball.seenState          = CompletelyOut;
        has_ball                = false;

        ownRobots_num = 0;
        for (int i = 0; i < Setting::kMaxRobots; i++)
        {
            OwnRobot[i].Angle              = 0.0f;
            OwnRobot[i].AngularVelocity    = 0.0f;
            OwnRobot[i].Position           = vec2(0.0f);
            OwnRobot[i].seenState          = CompletelyOut;
            OwnRobot[i].OutForSubsitute    = true;
            OwnRobot[i].velocity.direction = 0.0f;
            OwnRobot[i].velocity.magnitude = 0.0f;
            OwnRobot[i].velocity.x         = 0.0f;
            OwnRobot[i].velocity.y         = 0.0f;
            OwnRobot[i].vision_id          = i;
            for (int j = 0; j < 11; j++)
            {
                lastCMDS[i][j] = vec3(0.0f);
            }
        }
        oppRobots_num = 0;
        for (int i = 0; i < Setting::kMaxRobots; i++)
        { // TODO not in the last code...
            OppRobot[i].Angle              = 0.0f;
            OppRobot[i].AngularVelocity    = 0.0f;
            OppRobot[i].Position           = vec2(0.0f);
            OppRobot[i].seenState          = CompletelyOut;
            OppRobot[i].OutForSubsitute    = true;
            OppRobot[i].velocity.direction = 0.0f;
            OppRobot[i].velocity.magnitude = 0.0f;
            OppRobot[i].velocity.x         = 0.0f;
            OppRobot[i].velocity.y         = 0.0f;
            OppRobot[i].vision_id          = i;
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
