// game_state.h
//
// This class implements the transition system that defines the rules
// of the game.  The state changes based on the input from the referee
// and whether the ball has been kicked since the last referee
// command.  Calling the method transition() with these inputs will
// have the class maintain the state of the game.
//
// In addition, their are query methods for determining the current
// game state and what behavior is allowed in these game states.
//
// Created by:  Michael Bowling (mhb@cs.cmu.edu)
//
/* LICENSE: */

#pragma once

namespace Tyr::Referee
{
class Referee;
}

namespace Tyr::Common
{
class RefereeState
{
public:
    enum State
    {
        STATE_GAME_ON    = (1 << 0),
        STATE_GAME_OFF   = (1 << 1),
        STATE_HALTED     = (1 << 2),
        STATE_KICKOFF    = (1 << 3),
        STATE_PENALTY    = (1 << 4),
        STATE_DIRECT     = (1 << 5),
        STATE_INDIRECT   = (1 << 6),
        STATE_RESTART    = (STATE_KICKOFF | STATE_PENALTY | STATE_DIRECT | STATE_INDIRECT),
        STATE_READY      = (1 << 10),
        STATE_NOTREADY   = (1 << 11),
        STATE_PLACE_BALL = (1 << 12),
    };

    Vec2 place_ball_target;
    int  opp_gk = -1;

protected:
    friend class Referee::Referee;

    int       state = STATE_GAME_OFF;
    TeamColor color = TeamColor::Blue;

public:
    RefereeState() = default;

    State get() const
    {
        return (State) state;
    }

    bool our() const
    {
        return color == setting().our_color;
    }

    bool stop() const
    {
        return (state == STATE_GAME_OFF);
    }

    bool gameOn() const
    {
        return (state == STATE_GAME_ON);
    }

    bool restart() const
    {
        return (state & STATE_RESTART);
    }
    bool ourRestart() const
    {
        return restart() && our();
    }
    bool theirRestart() const
    {
        return restart() && !our();
    }

    bool kickoff() const
    {
        return (state & STATE_KICKOFF);
    }
    bool ourKickoff() const
    {
        return kickoff() && our();
    }
    bool theirKickoff() const
    {
        return kickoff() && !our();
    }

    bool penaltyKick() const
    {
        return (state & STATE_PENALTY);
    }
    bool ourPenaltyKick() const
    {
        return penaltyKick() && our();
    }
    bool theirPenaltyKick() const
    {
        return penaltyKick() && !our();
    }

    bool directKick() const
    {
        return (state & STATE_DIRECT);
    }
    bool ourDirectKick() const
    {
        return directKick() && our();
    }
    bool theirDirectKick() const
    {
        return directKick() && !our();
    }

    bool indirectKick() const
    {
        return (state & STATE_INDIRECT);
    }
    bool ourIndirectKick() const
    {
        return indirectKick() && our();
    }
    bool theirIndirectKick() const
    {
        return indirectKick() && !our();
    }

    bool placeBall() const
    {
        return (state & STATE_PLACE_BALL);
    }
    bool ourPlaceBall() const
    {
        return placeBall() && our();
    }
    bool theirPlaceBall() const
    {
        return placeBall() && !our();
    }

    bool freeKick() const
    {
        return directKick() || indirectKick();
    }
    bool ourFreeKick() const
    {
        return ourDirectKick() || ourIndirectKick();
    }
    bool theirFreeKick() const
    {
        return theirDirectKick() || theirIndirectKick();
    }

    bool canMove() const
    {
        return (state != STATE_HALTED);
    }

    bool allowedNearBall() const
    {
        return gameOn() || our();
    }

    bool canKickBall() const
    {
        return gameOn() || (ourRestart() && (state & STATE_READY));
    }
};
} // namespace Tyr::Common
