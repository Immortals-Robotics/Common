#pragma once

#include "draw.h"
#if FEATURE_LOGGING
#include "log.h"
#endif

#include "../time/time_point.h"

namespace Immortals::Common::Debug
{
struct Wrapper
{
    TimePoint time;

    std::vector<Draw> draws;

#if FEATURE_LOGGING
    std::vector<Log> logs;
#endif

    StringMap strings;

    Wrapper() = default;

    explicit Wrapper(const Protos::Immortals::Debug::Wrapper &t_wrapper)
    {
        time = TimePoint::fromMicroseconds(t_wrapper.time());

        strings.reserve(t_wrapper.strings().size());
        for (const auto &entry : t_wrapper.strings())
            strings.emplace(entry.first, entry.second);

        draws.reserve(t_wrapper.draw_size());

#if FEATURE_LOGGING
        logs.reserve(t_wrapper.log_size());
#endif

        for (const auto &draw : t_wrapper.draw())
            draws.emplace_back(draw, strings);

#if FEATURE_LOGGING
        for (const auto &log : t_wrapper.log())
            logs.emplace_back(log, strings);
#endif
    }

    void fillProto(Protos::Immortals::Debug::Wrapper *t_wrapper)
    {
        t_wrapper->set_time(time.microseconds());

        for (const auto &draw : draws)
            draw.fillProto(t_wrapper->add_draw(), &strings);

#if FEATURE_LOGGING
        for (const auto &log : logs)
            log.fillProto(t_wrapper->add_log(), &strings);
#endif

        for (const auto &entry : strings)
            t_wrapper->mutable_strings()->emplace(entry.first, entry.second);
    }
};
} // namespace Immortals::Common::Debug
