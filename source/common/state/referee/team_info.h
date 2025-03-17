#pragma once

namespace Immortals::Common::Referee
{
struct TeamInfo
{
    TeamInfo() = default;

    explicit TeamInfo(const Protos::Ssl::Gc::Referee_TeamInfo &t_team_info)
    {
        name = t_team_info.name();

        score = t_team_info.score();

        foul_counter = t_team_info.foul_counter();

        red_cards    = t_team_info.red_cards();
        yellow_cards = t_team_info.yellow_cards();

        yellow_cards_time_left.reserve(t_team_info.yellow_card_times_size());
        for (const uint64_t time : t_team_info.yellow_card_times())
        {
            yellow_cards_time_left.emplace_back(Duration::fromMicroseconds(time));
        }

        max_allowed_robots = t_team_info.max_allowed_bots();

        ball_placement_failures         = t_team_info.ball_placement_failures();
        ball_placement_failures_reached = t_team_info.ball_placement_failures_reached();
        can_place_ball                  = t_team_info.can_place_ball();

        timeouts_left     = t_team_info.timeouts();
        timeout_time_left = Duration::fromMicroseconds(t_team_info.timeout_time());

        gk_id = t_team_info.goalkeeper();

        substitution_intent    = t_team_info.bot_substitution_intent();
        substitution_allowed   = t_team_info.bot_substitution_allowed();
        substitutions_left     = t_team_info.bot_substitutions_left();
        substitution_time_left = Duration::fromMicroseconds(t_team_info.bot_substitution_time_left());
    }

    explicit TeamInfo(const Protos::Immortals::Referee::TeamInfo &t_team_info)
    {
        name = t_team_info.name();

        score = t_team_info.score();

        foul_counter = t_team_info.foul_counter();

        red_cards    = t_team_info.red_cards();
        yellow_cards = t_team_info.yellow_cards();

        yellow_cards_time_left.reserve(t_team_info.yellow_cards_time_left_size());
        for (const uint64_t time : t_team_info.yellow_cards_time_left())
        {
            yellow_cards_time_left.emplace_back(Duration::fromMicroseconds(time));
        }

        max_allowed_robots = t_team_info.max_allowed_robots();

        ball_placement_failures         = t_team_info.ball_placement_failures();
        ball_placement_failures_reached = t_team_info.ball_placement_failures_reached();
        can_place_ball                  = t_team_info.can_place_ball();

        timeouts_left     = t_team_info.timeouts_left();
        timeout_time_left = Duration::fromMicroseconds(t_team_info.timeout_time_left());

        gk_id = t_team_info.gk_id();

        substitution_intent    = t_team_info.substitution_intent();
        substitution_allowed   = t_team_info.substitution_allowed();
        substitutions_left     = t_team_info.substitutions_left();
        substitution_time_left = Duration::fromMicroseconds(t_team_info.substitution_time_left());
    }

    void fillProto(Protos::Immortals::Referee::TeamInfo *const t_team_info) const
    {
        t_team_info->set_name(name);

        t_team_info->set_score(score);

        t_team_info->set_foul_counter(foul_counter);

        t_team_info->set_red_cards(red_cards);
        t_team_info->set_yellow_cards(yellow_cards);

        for (const Duration &time : yellow_cards_time_left)
        {
            t_team_info->add_yellow_cards_time_left(time.microseconds());
        }

        t_team_info->set_max_allowed_robots(max_allowed_robots);

        t_team_info->set_ball_placement_failures(ball_placement_failures);
        t_team_info->set_ball_placement_failures_reached(ball_placement_failures_reached);
        t_team_info->set_can_place_ball(can_place_ball);

        t_team_info->set_timeouts_left(timeouts_left);
        t_team_info->set_timeout_time_left(timeout_time_left.microseconds());

        t_team_info->set_gk_id(gk_id);

        t_team_info->set_substitution_intent(substitution_intent);
        t_team_info->set_substitution_allowed(substitution_allowed);
        t_team_info->set_substitutions_left(substitutions_left);
        t_team_info->set_substitution_time_left(substitution_time_left.microseconds());
    }

    std::string name;

    unsigned score = 0;

    // The total number of countable fouls that act towards yellow cards
    unsigned foul_counter = 0;

    unsigned              red_cards    = 0;
    unsigned              yellow_cards = 0;
    std::vector<Duration> yellow_cards_time_left;

    unsigned max_allowed_robots = Config::Common::kMaxRobots;

    // The number of consecutive ball placement failures
    unsigned ball_placement_failures         = 0;
    bool     ball_placement_failures_reached = false;
    bool     can_place_ball                  = true;

    unsigned timeouts_left = 0;
    Duration timeout_time_left;

    // The pattern number of this team's goalkeeper.
    unsigned gk_id = 0;

    // The team has submitted an intent to substitute one or more robots at the next chance
    bool substitution_intent = false;
    // The team is allowed to substitute one or more robots currently
    bool substitution_allowed = false;
    // The number of bot substitutions left by the team in this halftime
    unsigned substitutions_left = 0;
    // The number of microseconds left for current bot substitution
    Duration substitution_time_left;
};
} // namespace Immortals::Common::Referee
