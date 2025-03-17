#pragma once

#include "command.h"
#include "match.h"
#include "team_info.h"

namespace Immortals::Common::Referee
{
// https://robocup-ssl.github.io/ssl-rules/sslrules.html#_game_states
enum class GameState
{
    None          = 0,
    Halt          = 1,
    Timeout       = 2,
    Stop          = 3,
    BallPlacement = 4,
    Kickoff       = 5,
    Penalty       = 6,
    FreeKick      = 7,
    Running       = 8,
};

struct State
{
    State() = default;

    explicit State(const Protos::Ssl::Gc::Referee &t_referee)
    {
        // time will be set by the Referee module

        match_type = static_cast<MatchType>(t_referee.match_type());

        stage           = static_cast<Stage>(t_referee.stage());
        stage_time_left = Duration::fromMicroseconds(t_referee.stage_time_left());

        // state, ready, color, and last_command is set by the Referee module

        if (t_referee.has_current_action_time_remaining())
        {
            time_remaining = Duration::fromMicroseconds(t_referee.current_action_time_remaining());
        }

        designated_position = static_cast<Vec2>(t_referee.designated_position());

        blue_info   = static_cast<TeamInfo>(t_referee.blue());
        yellow_info = static_cast<TeamInfo>(t_referee.yellow());

        const bool us_positive_half = config().common.our_color == TeamColor::Blue
                                          ? t_referee.blue_team_on_positive_half()
                                          : !t_referee.blue_team_on_positive_half();

        our_side = us_positive_half ? TeamSide::Right : TeamSide::Left;

        status_message = t_referee.status_message();
    }

    explicit State(const Protos::Immortals::Referee::State &t_state)
    {
        time = TimePoint::fromMicroseconds(t_state.time());

        match_type = static_cast<MatchType>(t_state.match_type());

        stage           = static_cast<Stage>(t_state.stage());
        stage_time_left = Duration::fromMicroseconds(t_state.stage_time_left());

        state          = static_cast<GameState>(t_state.state());
        ready          = t_state.ready();
        color          = static_cast<TeamColor>(t_state.color());
        time_remaining = Duration::fromMicroseconds(t_state.time_remaining());

        last_command = static_cast<Command>(t_state.last_command());

        designated_position = static_cast<Vec2>(t_state.designated_position());

        blue_info   = static_cast<TeamInfo>(t_state.blue_info());
        yellow_info = static_cast<TeamInfo>(t_state.yellow_info());

        our_side       = static_cast<TeamSide>(t_state.our_side());
        status_message = t_state.status_message();
    }

    void fillProto(Protos::Immortals::Referee::State *const t_state) const
    {
        t_state->set_time(time.microseconds());

        t_state->set_match_type(static_cast<Protos::Immortals::Referee::MatchType>(match_type));

        t_state->set_stage(static_cast<Protos::Immortals::Referee::Stage>(stage));
        t_state->set_stage_time_left(stage_time_left.microseconds());

        t_state->set_state(static_cast<Protos::Immortals::Referee::GameState>(state));
        t_state->set_ready(ready);
        t_state->set_color(static_cast<Protos::Immortals::TeamColor>(color));
        t_state->set_time_remaining(time_remaining.microseconds());

        last_command.fillProto(t_state->mutable_last_command());

        designated_position.fillProto(t_state->mutable_designated_position());

        blue_info.fillProto(t_state->mutable_blue_info());
        yellow_info.fillProto(t_state->mutable_yellow_info());

        t_state->set_our_side(static_cast<Protos::Immortals::TeamSide>(our_side));
        t_state->set_status_message(status_message);
    }

    // helper functions for different states
    bool our() const
    {
        return color == config().common.our_color;
    }

    bool halt() const
    {
        return state == GameState::Halt;
    }

    bool stop() const
    {
        return state == GameState::Stop;
    }

    bool running() const
    {
        return state == GameState::Running;
    }

    bool kickoff() const
    {
        return state == GameState::Kickoff;
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
        return state == GameState::Penalty;
    }
    bool ourPenaltyKick() const
    {
        return penaltyKick() && our();
    }
    bool theirPenaltyKick() const
    {
        return penaltyKick() && !our();
    }

    bool freeKick() const
    {
        return state == GameState::FreeKick;
    }
    bool ourFreeKick() const
    {
        return freeKick() && our();
    }
    bool theirFreeKick() const
    {
        return freeKick() && !our();
    }

    bool ballPlacement() const
    {
        return state == GameState::BallPlacement;
    }
    bool ourBallPlacement() const
    {
        return ballPlacement() && our();
    }
    bool theirBallPlacement() const
    {
        return ballPlacement() && !our();
    }

    bool restart() const
    {
        return state == GameState::Kickoff || state == GameState::Penalty || state == GameState::FreeKick;
    }
    bool ourRestart() const
    {
        return restart() && our();
    }
    bool theirRestart() const
    {
        return restart() && !our();
    }

    bool timeout() const
    {
        return state == GameState::Timeout;
    }
    bool ourTimeout() const
    {
        return timeout() && our();
    }
    bool theirTimeout() const
    {
        return timeout() && !our();
    }

    // limitations in different states according to the rules
    bool canMove() const
    {
        return !halt();
    }

    bool allowedNearBall() const
    {
        return running() || ourRestart() || ourBallPlacement();
    }

    bool canKickBall() const
    {
        return running() || (ourRestart() && ready);
    }

    bool shouldSlowDown() const
    {
        return stop() || ballPlacement() || canEnterField();
    }

    bool canEnterField() const
    {
        return timeout();
    }

    // misc helper functions
    const TeamInfo &ourInfo() const
    {
        return config().common.our_color == TeamColor::Blue ? blue_info : yellow_info;
    }

    const TeamInfo &oppInfo() const
    {
        return config().common.our_color == TeamColor::Yellow ? blue_info : yellow_info;
    }

    // time elapsed since transition to this state
    Duration elapsed() const
    {
        return TimePoint::now() - time;
    }

    // when transitioned to this state
    TimePoint time;

    MatchType match_type = MatchType::Unknown;

    Stage    stage = Stage::Unknown;
    Duration stage_time_left;

    GameState state = GameState::None;
    bool      ready = false;
    TeamColor color = TeamColor::Blue;
    Duration  time_remaining;

    Command last_command;

    Vec2 designated_position;

    TeamInfo blue_info;
    TeamInfo yellow_info;

    TeamSide our_side = TeamSide::Left;

    std::string status_message;
};
} // namespace Immortals::Common::Referee

#if FEATURE_LOGGING
template <>
struct fmt::formatter<Immortals::Common::Referee::State> : fmt::formatter<std::string>
{
    auto format(const Immortals::Common::Referee::State &t_state, format_context &t_ctx) const
    {
        const char *state_str = "Unknown";
        switch (t_state.state)
        {
        case Immortals::Common::Referee::GameState::None:
            state_str = "None";
            break;
        case Immortals::Common::Referee::GameState::Halt:
            state_str = "Halt";
            break;
        case Immortals::Common::Referee::GameState::Timeout:
            state_str = t_state.our() ? "Our timeout" : "Their timeout";
            break;
        case Immortals::Common::Referee::GameState::Stop:
            state_str = "Stop";
            break;
        case Immortals::Common::Referee::GameState::BallPlacement:
            state_str = t_state.our() ? "Our ball placement" : "Their ball placement";
            break;
        case Immortals::Common::Referee::GameState::Kickoff:
            state_str = t_state.our() ? (t_state.ready ? "Our kickoff (ready)" : "Our prepare kickoff")
                                      : (t_state.ready ? "Their kickoff (ready)" : "Their prepare kickoff");
            break;
        case Immortals::Common::Referee::GameState::Penalty:
            state_str = t_state.our() ? (t_state.ready ? "Our penalty (ready)" : "Our prepare penalty")
                                      : (t_state.ready ? "Their penalty (ready)" : "Their prepare penalty");
            break;
        case Immortals::Common::Referee::GameState::FreeKick:
            state_str = t_state.our() ? "Our free kick" : "Their free kick";
            break;
        case Immortals::Common::Referee::GameState::Running:
            state_str = "Running";
            break;
        }

        return fmt::format_to(t_ctx.out(), "{}", state_str);
    }
};
#endif
