#pragma once

#include "game.h"

namespace Tyr::Common
{
struct RefereeState
{
    GameState state;

    unsigned short time_remaining = 0;
    unsigned short goals_blue     = 0;
    unsigned short goals_yellow   = 0;
    unsigned char  counter        = 0;
    Vec2           place_ball_target;
    int            opp_gk = -1;
};
} // namespace Tyr::Common
