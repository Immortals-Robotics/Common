#pragma once

#include "command.h"
#include "match.h"
#include "team_info.h"

namespace Immortals::Referee
{
class Referee;
}

namespace Immortals::Common::Referee
{
class State
{
public:
    State() = default;

    explicit State(const Protos::Immortals::Referee::State &t_state)
    {
        time_sent    = TimePoint::fromMicroseconds(t_state.time_sent());
        time_gc_sent = TimePoint::fromMicroseconds(t_state.time_gc_sent());

        source_uuid = t_state.source_uuid();

        match_type = static_cast<MatchType>(t_state.match_type());

        stage           = static_cast<Stage>(t_state.stage());
        stage_time_left = Duration::fromMicroseconds(t_state.stage_time_left());

        command = static_cast<Command>(t_state.command());

        place_ball_target = static_cast<Vec2>(t_state.place_ball_target());

        blue_info   = static_cast<TeamInfo>(t_state.blue_info());
        yellow_info = static_cast<TeamInfo>(t_state.yellow_info());
    }

    void fillProto(Protos::Immortals::Referee::State *const t_state) const
    {
        t_state->set_time_sent(time_sent.microseconds());
        t_state->set_time_gc_sent(time_gc_sent.microseconds());

        t_state->set_source_uuid(source_uuid);

        t_state->set_match_type(static_cast<Protos::Immortals::Referee::MatchType>(match_type));

        t_state->set_stage(static_cast<Protos::Immortals::Referee::Stage>(stage));
        t_state->set_stage_time_left(stage_time_left.microseconds());

        command.fillProto(t_state->mutable_command());

        place_ball_target.fillProto(t_state->mutable_place_ball_target());

        blue_info.fillProto(t_state->mutable_blue_info());
        yellow_info.fillProto(t_state->mutable_yellow_info());
    }

    // when this state was sent on our side
    TimePoint time_sent;
    // when this state was sent from game controller
    TimePoint time_gc_sent;

    std::string source_uuid;

    MatchType match_type = MatchType::Unknown;

    Stage    stage = Stage::None;
    Duration stage_time_left;

    Command command;

    Vec2 place_ball_target;

    TeamInfo blue_info;
    TeamInfo yellow_info;
};
} // namespace Immortals::Common::Referee
