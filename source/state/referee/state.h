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

        designated_position = static_cast<Vec2>(t_referee.designated_position());

        blue_info   = static_cast<TeamInfo>(t_referee.blue());
        yellow_info = static_cast<TeamInfo>(t_referee.yellow());
    }

    explicit State(const Protos::Immortals::Referee::State &t_state)
    {
        time = TimePoint::fromMicroseconds(t_state.time());

        match_type = static_cast<MatchType>(t_state.match_type());

        stage           = static_cast<Stage>(t_state.stage());
        stage_time_left = Duration::fromMicroseconds(t_state.stage_time_left());

        state = static_cast<GameState>(t_state.state());
        ready = t_state.ready();
        color = static_cast<TeamColor>(t_state.color());

        last_command = static_cast<Command>(t_state.last_command());

        designated_position = static_cast<Vec2>(t_state.designated_position());

        blue_info   = static_cast<TeamInfo>(t_state.blue_info());
        yellow_info = static_cast<TeamInfo>(t_state.yellow_info());
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

        last_command.fillProto(t_state->mutable_last_command());

        designated_position.fillProto(t_state->mutable_designated_position());

        blue_info.fillProto(t_state->mutable_blue_info());
        yellow_info.fillProto(t_state->mutable_yellow_info());
    }

    bool our() const
    {
        return color == config().common.our_color;
    }

    bool restart() const
    {
        return state == GameState::Kickoff || state == GameState::Penalty || state == GameState::FreeKick;
    }

    bool canMove() const
    {
        return state != GameState::Halt;
    }

    bool allowedNearBall() const
    {
        return state == GameState::Running || ((restart() || state == GameState::BallPlacement) && our());
    }

    bool canKickBall() const
    {
        return state == GameState::Running || (restart() && our() && ready);
    }

    bool shouldSlowDown() const
    {
        return state == GameState::Stop;
    }

    // when transitioned to this state
    TimePoint time;

    MatchType match_type = MatchType::Unknown;

    Stage    stage = Stage::Unknown;
    Duration stage_time_left;

    GameState state = GameState::None;
    bool      ready = false;
    TeamColor color = TeamColor::Blue;

    Command last_command;

    Vec2 designated_position;

    TeamInfo blue_info;
    TeamInfo yellow_info;
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

        return fmt::format_to(t_ctx.out(), "[{}] {}", t_state.time, state_str);
    }
};
#endif
