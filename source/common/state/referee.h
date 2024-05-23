#pragma once

#include "../time/time_point.h"

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

    TimePoint time;

    const static inline std::unordered_map<int, std::string> m_state_string = {
        {0, "None"},
        {STATE_GAME_ON, "Game on"},
        {STATE_GAME_OFF, "Game off"},
        {STATE_HALTED, "HALT"},
        {STATE_KICKOFF, "Kickoff"},
        {STATE_PENALTY, "Penalty"},
        {STATE_DIRECT, "Direct free kick"},
        {STATE_INDIRECT, "Indirect free kick"},
        {STATE_PLACE_BALL, "Ball placement"},
    };

    int                      state = STATE_GAME_OFF;
    std::optional<TeamColor> color;
    Vec2                     place_ball_target;
    int                      opp_gk = -1;

public:
    RefereeState() = default;

    RefereeState(const Protos::Immortals::RefereeState &t_state)
    {
        time = TimePoint::fromMicroseconds(t_state.time());

        state = t_state.state();
        if (t_state.has_color())
            color = static_cast<TeamColor>(t_state.color());
        place_ball_target = Common::Vec2{t_state.place_ball_target()};
        opp_gk            = t_state.opp_gk();
    }

    void fillProto(Protos::Immortals::RefereeState *const t_state) const
    {
        t_state->set_time(time.microseconds());

        t_state->set_state(state);
        if (color.has_value())
            t_state->set_color(static_cast<Protos::Immortals::TeamColor>(color.value()));
        place_ball_target.fillProto(t_state->mutable_place_ball_target());
        t_state->set_opp_gk(opp_gk);
    }

    State get() const
    {
        return static_cast<State>(state);
    }

    bool our() const
    {
        return color == config().common.our_color;
    }

    std::string getString() const
    {
        auto state_str = RefereeState::m_state_string.at(
            state & (STATE_GAME_ON | STATE_GAME_OFF | STATE_HALTED | STATE_RESTART | STATE_PLACE_BALL));
        if (state & STATE_RESTART)
        {
            state_str = (our() ? "Our " : "Their ") + state_str;
        }
        return state_str;
    }

    bool stop() const
    {
        return state == STATE_GAME_OFF;
    }

    bool gameOn() const
    {
        return state == STATE_GAME_ON;
    }

    bool restart() const
    {
        return state & STATE_RESTART;
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
        return state & STATE_KICKOFF;
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
        return state & STATE_PENALTY;
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
        return state & STATE_DIRECT;
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
        return state & STATE_INDIRECT;
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
        return state & STATE_PLACE_BALL;
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
        return state != STATE_HALTED;
    }

    bool allowedNearBall() const
    {
        return gameOn() || our();
    }

    bool canKickBall() const
    {
        return gameOn() || (ourRestart() && (state & STATE_READY));
    }

    // TODO: check this in the rules
    bool shouldSlowDown() const
    {
        return stop();
    }
};
} // namespace Tyr::Common
