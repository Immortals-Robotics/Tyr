#pragma once

namespace Tyr::Common::Referee
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
    Unknown = -1,

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
} // namespace Tyr::Common::Referee

template <>
struct fmt::formatter<Tyr::Common::Referee::MatchType> : fmt::formatter<std::string>
{
    auto format(const Tyr::Common::Referee::MatchType t_type, format_context &t_ctx) const
    {
        const char *type_str = "Unknown";
        switch (t_type)
        {
        case Tyr::Common::Referee::MatchType::Unknown:
            type_str = "Unknown";
            break;
        case Tyr::Common::Referee::MatchType::GroupPhase:
            type_str = "Group phase";
            break;
        case Tyr::Common::Referee::MatchType::EliminationPhase:
            type_str = "Elimination phase";
            break;
        case Tyr::Common::Referee::MatchType::Friendly:
            type_str = "Friendly";
            break;
        }

        return fmt::format_to(t_ctx.out(), "{}", type_str);
    }
};

template <>
struct fmt::formatter<Tyr::Common::Referee::Stage> : fmt::formatter<std::string>
{
    auto format(const Tyr::Common::Referee::Stage t_stage, format_context &t_ctx) const
    {
        const char *stage_str = "Unknown";
        switch (t_stage)
        {
        case Tyr::Common::Referee::Stage::Unknown:
            stage_str = "Unknown";
            break;
        case Tyr::Common::Referee::Stage::NormalFirstHalfPre:
            stage_str = "First Half Pre";
            break;
        case Tyr::Common::Referee::Stage::NormalFirstHalf:
            stage_str = "First Half";
            break;
        case Tyr::Common::Referee::Stage::NormalHalfTime:
            stage_str = "Half Time";
            break;
        case Tyr::Common::Referee::Stage::NormalSecondHalfPre:
            stage_str = "Second Half Pre";
            break;
        case Tyr::Common::Referee::Stage::NormalSecondHalf:
            stage_str = "Second Half";
            break;
        case Tyr::Common::Referee::Stage::ExtraTimeBreak:
            stage_str = "Extra Time Break";
            break;
        case Tyr::Common::Referee::Stage::ExtraFirstHalfPre:
            stage_str = "Extra First Half Pre";
            break;
        case Tyr::Common::Referee::Stage::ExtraFirstHalf:
            stage_str = "Extra First Half";
            break;
        case Tyr::Common::Referee::Stage::ExtraHalfTime:
            stage_str = "Extra Half Time";
            break;
        case Tyr::Common::Referee::Stage::ExtraSecondHalfPre:
            stage_str = "Extra Second Half Pre";
            break;
        case Tyr::Common::Referee::Stage::ExtraSecondHalf:
            stage_str = "Extra Second Half";
            break;
        case Tyr::Common::Referee::Stage::PenaltyShootoutBreak:
            stage_str = "Penalty Shootout Break";
            break;
        case Tyr::Common::Referee::Stage::PenaltyShootout:
            stage_str = "Penalty Shootout";
            break;
        case Tyr::Common::Referee::Stage::PostGame:
            stage_str = "Post Game";
            break;
        }

        return fmt::format_to(t_ctx.out(), "{}", stage_str);
    }
};
