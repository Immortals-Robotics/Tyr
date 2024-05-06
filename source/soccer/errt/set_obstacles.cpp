#include "../ai.h"

namespace Tyr::Soccer
{
static constexpr float ballAreaRadius = 550.0f;

// We allow errt points to be 250 mm outside the field,
// so set this to some higher value
static constexpr float penaltyAreaExtensionBehindGoal = 300.0f;
static constexpr float bigPenaltyAddition             = 300.0f;

static float calculateRobotRadius(const Common::RobotState &state)
{
    const float extension_factor = std::min(1.0f, state.velocity.length() / 10000.0f);
    return Common::field().ball_radius * (1.0f + extension_factor);
}

void Ai::ERRTSetObstacles(int robot_num, bool bll, bool field)
{
    const bool ourPenalty = field || (robot_num != gk && !Common::refereeState().ourPlaceBall());
    const bool oppPenalty = field || !Common::refereeState().ourPlaceBall();

    const bool oppPenaltyBig = Common::refereeState().freeKick() || Common::refereeState().stop();

    const float current_robot_radius = calculateRobotRadius(OwnRobot[robot_num].state());

    obs_map.resetMap();

    // own
    for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
    {
        if ((OwnRobot[i].state().seen_state != Common::SeenState::CompletelyOut) && (i != robot_num) &&
            (OwnRobot[i].state().vision_id != OwnRobot[robot_num].state().vision_id))
        {
            obs_map.addCircle(
                {OwnRobot[i].state().position, current_robot_radius + Common::field().robot_radius});
            // Common::debug().draw({OwnRobot[i].state().position,ownRobotRadius + (!dribble)*ownRobotRadius},Cyan);
        }
    }

    // opp
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (Common::worldState().opp_robot[i].seen_state != Common::SeenState::CompletelyOut)
        {
            const float radius = calculateRobotRadius(Common::worldState().opp_robot[i]);

            obs_map.addCircle({Common::worldState().opp_robot[i].position, radius + current_robot_radius});
            // Common::debug().draw({Common::worldState().opp_robot[i].position,ownRobotRadius +
            // (!dribble)*ownRobotRadius},Cyan);
        }
    }

    if (bll || !Common::refereeState().allowedNearBall())
    {
        obs_map.addCircle({Common::worldState().ball.position, ballAreaRadius + current_robot_radius});
    }

    const float penalty_area_half_width = Common::field().penalty_area_width / 2.0f;

    if (ourPenalty)
    {
        const Common::Vec2 start{side * (Common::field().width + penaltyAreaExtensionBehindGoal),
                                 -(penalty_area_half_width + current_robot_radius)};

        const float w = -side * (penaltyAreaExtensionBehindGoal + current_robot_radius +
                                 Common::field().penalty_area_depth);
        const float h = Common::field().penalty_area_width + 2 * current_robot_radius;

        obs_map.addRectangle({start, w, h});
    }

    if (oppPenalty)
    {
        const Common::Vec2 start{-side * (Common::field().width + penaltyAreaExtensionBehindGoal),
                                 -(penalty_area_half_width + current_robot_radius)};

        const float w = side * (penaltyAreaExtensionBehindGoal + current_robot_radius +
                                Common::field().penalty_area_depth);
        const float h = Common::field().penalty_area_width + 2 * current_robot_radius;

        obs_map.addRectangle({start, w, h});
    }

    if (oppPenaltyBig)
    {
        const float big_penalty_area_r      = Common::field().penalty_area_depth + bigPenaltyAddition;
        const float big_penalty_area_w      = Common::field().penalty_area_width + bigPenaltyAddition;
        const float penalty_area_half_width = big_penalty_area_w / 2.0f;

        const Common::Vec2 start{-side * (Common::field().width + penaltyAreaExtensionBehindGoal),
                                 -(penalty_area_half_width + current_robot_radius)};

        const float w = side * (penaltyAreaExtensionBehindGoal + current_robot_radius + big_penalty_area_r);
        const float h = big_penalty_area_w + 2 * current_robot_radius;

        obs_map.addRectangle({start, w, h});
    }

    // avoid the line between the ball and the placement point
    if (Common::refereeState().theirPlaceBall())
    {
        const Common::Vec2 ball_line = Common::refereeState().place_ball_target - Common::worldState().ball.position;
        const int          ball_obs_count = std::ceil(ball_line.length() / (ballAreaRadius + current_robot_radius));

        for (int i = 0; i < ball_obs_count; i++)
        {
            const float        t        = (float) i / (float) ball_obs_count;
            const Common::Vec2 ball_obs = Common::worldState().ball.position + ball_line * t;
            obs_map.addCircle({ball_obs, ballAreaRadius + current_robot_radius});
        }
    }
}

void Ai::ERRTSetGkClearObstacles(int robot_num)
{
    obs_map.resetMap();

    // our penalty area
    static constexpr float area_extension_size     = 200.0f;
    const float            penalty_area_half_width = Common::field().penalty_area_width / 2.0f;

    const Common::Vec2 start{side * Common::field().width, -(penalty_area_half_width + area_extension_size)};

    const float w = -side * (area_extension_size + Common::field().penalty_area_depth);
    const float h = Common::field().penalty_area_width + 2 * area_extension_size;

    obs_map.addRectangle({start, w, h});
}
} // namespace Tyr::Soccer