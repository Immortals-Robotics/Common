#pragma once

namespace Immortals::Common::Referee
{
struct Command
{
    enum Type
    {
        None      = 0,
        GameOn    = 1 << 0,
        GameOff   = 1 << 1,
        Halt      = 1 << 2,
        Kickoff   = 1 << 3,
        Penalty   = 1 << 4,
        Direct    = 1 << 5,
        PlaceBall = 1 << 6,
        Restart   = Kickoff | Penalty | Direct,
        Sided     = Kickoff | Penalty | Direct | PlaceBall,
        Ready     = 1 << 7,
        Notready  = 1 << 8,
    };

    Command() = default;

    explicit Command(const Protos::Immortals::Referee::Command &t_command)
    {
        id   = t_command.id();
        time = TimePoint::fromMicroseconds(t_command.time());

        type = static_cast<Type>(t_command.type());

        if (t_command.has_color())
        {
            color = static_cast<TeamColor>(t_command.color());
        }
    }

    void fillProto(Protos::Immortals::Referee::Command *const t_command) const
    {
        t_command->set_id(id);
        t_command->set_time(time.microseconds());

        t_command->set_type(type);

        if (color.has_value())
        {
            t_command->set_color(static_cast<Protos::Immortals::TeamColor>(color.value()));
        }
    }

    Type get() const
    {
        return type;
    }

    bool our() const
    {
        return color == config().common.our_color;
    }

    bool stop() const
    {
        return type & GameOff;
    }

    bool halt() const
    {
        return type & Halt;
    }

    bool gameOn() const
    {
        return type & GameOn;
    }

    bool restart() const
    {
        return type & Restart;
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
        return type & Kickoff;
    }
    bool ourKickoff() const
    {
        return kickoff() && our();
    }
    bool theirKickoff() const
    {
        return kickoff() && !our();
    }

    bool penalty() const
    {
        return type & Penalty;
    }
    bool ourPenaltyKick() const
    {
        return penalty() && our();
    }
    bool theirPenaltyKick() const
    {
        return penalty() && !our();
    }

    bool directKick() const
    {
        return type & Direct;
    }
    bool ourDirectKick() const
    {
        return directKick() && our();
    }
    bool theirDirectKick() const
    {
        return directKick() && !our();
    }

    bool placeBall() const
    {
        return type & PlaceBall;
    }
    bool ourPlaceBall() const
    {
        return placeBall() && our();
    }
    bool theirPlaceBall() const
    {
        return placeBall() && !our();
    }

    bool canMove() const
    {
        return !halt();
    }

    bool allowedNearBall() const
    {
        return gameOn() || our();
    }

    bool canKickBall() const
    {
        return gameOn() || (ourRestart() && type & Ready);
    }

    // TODO: check this in the rules
    bool shouldSlowDown() const
    {
        return stop();
    }

    std::string_view toString() const
    {
        if (gameOn())
            return "Game on";
        if (stop())
            return "Stop";
        if (halt())
            return "Halt";
        if (kickoff())
            return our() ? "Our kickoff" : "Their kickoff";
        if (penalty())
            return our() ? "Our penalty" : "Their penalty";
        if (directKick())
            return our() ? "Our direct free kick" : "Their direct free kick";
        if (placeBall())
            return our() ? "Our ball placement" : "Their ball placement";
    }

    unsigned  id;
    TimePoint time;

    Type                     type = Halt;
    std::optional<TeamColor> color;
};
} // namespace Immortals::Common::Referee

#if FEATURE_LOGGING
template <>
struct fmt::formatter<Immortals::Common::Referee::Command> : fmt::formatter<std::string>
{
    auto format(Immortals::Common::Referee::Command t_cmd, format_context &t_ctx) const
    {
        return fmt::format_to(t_ctx.out(), "{}", t_cmd.toString());
    }
};
#endif
