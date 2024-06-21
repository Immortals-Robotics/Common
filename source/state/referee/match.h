#pragma once

namespace Immortals::Common::Referee
{
enum class MatchType
{
    Unknown          = 0,
    GroupPhase       = 1,
    EliminationPhase = 2,
    Friendly         = 3,
};

enum class Stage
{
    None = -1,

    // The first half is about to start.
    // A kickoff is called within this stage.
    // This stage ends with the NORMAL_START.
    NormalFirstHalfPre = 0,
    // The first half of the normal game, before half time.
    NormalFirstHalf = 1,
    // Half time between first and second halves.
    NormalHalfTime = 2,
    // The second half is about to start.
    // A kickoff is called within this stage.
    // This stage ends with the NORMAL_START.
    NormalSecondHalfPre = 3,
    // The second half of the normal game, after half time.
    NormalSecondHalf = 4,
    // The break before extra time.
    ExtraTimeBreak = 5,
    // The first half of extra time is about to start.
    // A kickoff is called within this stage.
    // This stage ends with the NORMAL_START.
    ExtraFirstHalfPre = 6,
    // The first half of extra time.
    ExtraFirstHalf = 7,
    // Half time between first and second extra halves.
    ExtraHalfTime = 8,
    // The second half of extra time is about to start.
    // A kickoff is called within this stage.
    // This stage ends with the NORMAL_START.
    ExtraSecondHalfPre = 9,
    // The second half of extra time.
    ExtraSecondHalf = 10,
    // The break before penalty shootout.
    PenaltyShootoutBreak = 11,
    // The penalty shootout.
    PenaltyShootout = 12,
    // The game is over.
    PostGame = 13,
};
} // namespace Immortals::Common::Referee
