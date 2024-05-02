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

namespace Tyr::Common
{
class GameState
{
public:
    static const int STATE_GAME_ON  = (1 << 0);
    static const int STATE_GAME_OFF = (1 << 1);
    static const int STATE_HALTED   = (1 << 2);

    static const int STATE_KICKOFF  = (1 << 3);
    static const int STATE_PENALTY  = (1 << 4);
    static const int STATE_DIRECT   = (1 << 5);
    static const int STATE_INDIRECT = (1 << 6);
    static const int STATE_RESTART  = (STATE_KICKOFF | STATE_PENALTY | STATE_DIRECT | STATE_INDIRECT);

    static const int STATE_BLUE   = (1 << 8);
    static const int STATE_YELLOW = (1 << 9);

    static const int STATE_READY    = (1 << 10);
    static const int STATE_NOTREADY = (1 << 11);

    static const int STATE_PLACE_BALL = (1 << 12);

    int state;

    // The set of possible states are:
    //
    // { STATE_GAME_ON, STATE_GAME_OFF, STATE_HALTED, NEUTRAL,
    //   { { STATE_KICKOFF, STATE_PENALTY, STATE_DIRECT, STATE_INDIRECT } |
    //     { STATE_BLUE, STATE_YELLOW } | { STATE_READY, STATE_NOTREADY } } }
    //

    int color;

public:
    GameState()
    {
        color = STATE_BLUE;
        state = STATE_GAME_OFF;
    }

    void init(TeamColor color)
    {
        this->color = (color == TeamColor::Blue) ? STATE_BLUE : STATE_YELLOW;
    }

    int get()
    {
        return state;
    }
    void set(int _state)
    {
        state = _state;
    }

    // This is the state machine transition function.  It takes the last
    // ref_command as input
    void transition(char ref_command, bool ball_kicked)
    {
        if (ref_command == Protos::SSL_Referee_Command_HALT)
        {
            state = STATE_HALTED;
            return;
        }

        if (ref_command == Protos::SSL_Referee_Command_STOP)
        {
            state = STATE_GAME_OFF;
            return;
        }

        if (ref_command == Protos::SSL_Referee_Command_FORCE_START)
        {
            state = STATE_GAME_ON;
            return;
        }

        if (ref_command == Protos::SSL_Referee_Command_NORMAL_START && (state & STATE_NOTREADY))
        {
            state &= ~STATE_NOTREADY;
            state |= STATE_READY;
            return;
        }

        if (state & STATE_READY && ball_kicked)
        {
            state = STATE_GAME_ON;
            return;
        }

        if (state == STATE_GAME_OFF)
        {
            switch (ref_command)
            {
            case Protos::SSL_Referee_Command_PREPARE_KICKOFF_BLUE:
                state = STATE_KICKOFF | STATE_BLUE | STATE_NOTREADY;
                return;
            case Protos::SSL_Referee_Command_PREPARE_KICKOFF_YELLOW:
                state = STATE_KICKOFF | STATE_YELLOW | STATE_NOTREADY;
                return;

            case Protos::SSL_Referee_Command_PREPARE_PENALTY_BLUE:
                state = STATE_PENALTY | STATE_BLUE | STATE_NOTREADY;
                return;
            case Protos::SSL_Referee_Command_PREPARE_PENALTY_YELLOW:
                state = STATE_PENALTY | STATE_YELLOW | STATE_NOTREADY;
                return;

            case Protos::SSL_Referee_Command_DIRECT_FREE_BLUE:
                state = STATE_DIRECT | STATE_BLUE | STATE_READY;
                return;
            case Protos::SSL_Referee_Command_DIRECT_FREE_YELLOW:
                state = STATE_DIRECT | STATE_YELLOW | STATE_READY;
                return;

            case Protos::SSL_Referee_Command_INDIRECT_FREE_BLUE:
                state = STATE_INDIRECT | STATE_BLUE | STATE_READY;
                return;
            case Protos::SSL_Referee_Command_INDIRECT_FREE_YELLOW:
                state = STATE_INDIRECT | STATE_YELLOW | STATE_READY;
                return;

            case Protos::SSL_Referee_Command_BALL_PLACEMENT_BLUE:
                state = STATE_PLACE_BALL | STATE_BLUE | STATE_NOTREADY;
                return;
            case Protos::SSL_Referee_Command_BALL_PLACEMENT_YELLOW:
                state = STATE_PLACE_BALL | STATE_YELLOW | STATE_NOTREADY;
                return;

            default:
                break;
            }
        }
    }

    bool stop()
    {
        return (state == STATE_GAME_OFF);
    }

    bool gameOn()
    {
        return (state == STATE_GAME_ON);
    }

    bool restart()
    {
        return (state & STATE_RESTART);
    }
    bool ourRestart()
    {
        return restart() && (state & color);
    }
    bool theirRestart()
    {
        return restart() && !(state & color);
    }

    bool kickoff()
    {
        return (state & STATE_KICKOFF);
    }
    bool ourKickoff()
    {
        return kickoff() && (state & color);
    }
    bool theirKickoff()
    {
        return kickoff() && !(state & color);
    }

    bool penaltyKick()
    {
        return (state & STATE_PENALTY);
    }
    bool ourPenaltyKick()
    {
        return penaltyKick() && (state & color);
    }
    bool theirPenaltyKick()
    {
        return penaltyKick() && !(state & color);
    }

    bool directKick()
    {
        return (state & STATE_DIRECT);
    }
    bool ourDirectKick()
    {
        return directKick() && (state & color);
    }
    bool theirDirectKick()
    {
        return directKick() && !(state & color);
    }

    bool indirectKick()
    {
        return (state & STATE_INDIRECT);
    }
    bool ourIndirectKick()
    {
        return indirectKick() && (state & color);
    }
    bool theirIndirectKick()
    {
        return indirectKick() && !(state & color);
    }

    bool placeBall()
    {
        return (state & STATE_PLACE_BALL);
    }
    bool ourPlaceBall()
    {
        return placeBall() && (state & color);
    }
    bool theirPlaceBall()
    {
        return placeBall() && !(state & color);
    }

    bool freeKick()
    {
        return directKick() || indirectKick();
    }
    bool ourFreeKick()
    {
        return ourDirectKick() || ourIndirectKick();
    }
    bool theirFreeKick()
    {
        return theirDirectKick() || theirIndirectKick();
    }

    bool canMove()
    {
        return (state != STATE_HALTED);
    }

    bool allowedNearBall()
    {
        return gameOn() || (state & color);
    }

    bool canKickBall()
    {
        return gameOn() || (ourRestart() && (state & STATE_READY));
    }
};
} // namespace Tyr::Common
