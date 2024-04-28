#pragma once

namespace Tyr::Common
{
struct RefereeState
{
    GameState     *State;
    unsigned short time_remaining;
    unsigned short goals_blue;
    unsigned short goals_yellow;
    unsigned char  counter;
    Vec2           placeBallTargetPosition;
    int            oppGK;

    RefereeState()
    {
        State                   = new GameState;
        time_remaining          = 0;
        goals_blue              = 0;
        goals_yellow            = 0;
        counter                 = 0;
        placeBallTargetPosition = Vec2();
    }
};
} // namespace Tyr::Common
