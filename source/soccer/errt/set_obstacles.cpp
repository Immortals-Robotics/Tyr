#include "../ai.h"

namespace Tyr::Soccer
{
static constexpr float robotRadius = 92.0f;

static constexpr float ballAreaRadius = 550.0f;

// We allow errt points to be 250 mm outside the field,
// so set this to some higher value
static constexpr float penaltyAreaExtensionBehindGoal = 300.0f;
static constexpr float bigPenaltyAddition             = 300.0f;

static float calculateRobotRadius(const Common::RobotState &state)
{
    const float extension_factor = std::min(1.0f, state.velocity.length() / 10000.0f);
    return robotRadius * (1.0f + extension_factor);
}

void Ai::ERRTSetObstacles(int robot_num, bool bll, bool field)
{
    const bool ourPenalty = field || (robot_num != gk && !refereeState->ourPlaceBall());
    const bool oppPenalty = field || !refereeState->ourPlaceBall();

    const bool oppPenaltyBig = refereeState->freeKick() || refereeState->stop();

    const float current_robot_radius = calculateRobotRadius(OwnRobot[robot_num].State);

    obs_map.resetMap();

    // own
    for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
    {
        if ((OwnRobot[i].State.seen_state != Common::SeenState::CompletelyOut) && (i != robot_num) &&
            (OwnRobot[i].State.vision_id != OwnRobot[robot_num].State.vision_id))
        {
            obs_map.addCircle({OwnRobot[i].State.position, current_robot_radius + robotRadius});
            // Common::debug().drawCircle(OwnRobot[i].State.position,ownRobotRadius + (!dribble)*ownRobotRadius,Cyan);
        }
    }

    // opp
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (OppRobot[i].seen_state != Common::SeenState::CompletelyOut)
        {
            const float radius = calculateRobotRadius(OppRobot[i]);

            obs_map.addCircle({OppRobot[i].position, radius + current_robot_radius});
            // Common::debug().drawCircle(OppRobot[i].position,ownRobotRadius + (!dribble)*ownRobotRadius,Cyan);
        }
    }

    if (bll || !refereeState->allowedNearBall())
    {
        obs_map.addCircle({ball.position, ballAreaRadius + current_robot_radius});
    }

    const float penalty_area_half_width = penalty_area_width / 2.0f;

    if (ourPenalty)
    {
        const Common::Vec2 start{side * (field_width + penaltyAreaExtensionBehindGoal),
                                 -(penalty_area_half_width + current_robot_radius)};

        const float w = -side * (penaltyAreaExtensionBehindGoal + current_robot_radius + penalty_area_r);
        const float h = penalty_area_width + 2 * current_robot_radius;

        obs_map.addRectangle({start, w, h});
    }

    if (oppPenalty)
    {
        const Common::Vec2 start{-side * (field_width + penaltyAreaExtensionBehindGoal),
                                 -(penalty_area_half_width + current_robot_radius)};

        const float w = side * (penaltyAreaExtensionBehindGoal + current_robot_radius + penalty_area_r);
        const float h = penalty_area_width + 2 * current_robot_radius;

        obs_map.addRectangle({start, w, h});
    }

    if (oppPenaltyBig)
    {
        const float big_penalty_area_r      = penalty_area_r + bigPenaltyAddition;
        const float big_penalty_area_w      = penalty_area_width + bigPenaltyAddition;
        const float penalty_area_half_width = big_penalty_area_w / 2.0f;

        const Common::Vec2 start{-side * (field_width + penaltyAreaExtensionBehindGoal),
                                 -(penalty_area_half_width + current_robot_radius)};

        const float w = side * (penaltyAreaExtensionBehindGoal + current_robot_radius + big_penalty_area_r);
        const float h = big_penalty_area_w + 2 * current_robot_radius;

        obs_map.addRectangle({start, w, h});
    }

    // avoid the line between the ball and the placement point
    if (refereeState->theirPlaceBall())
    {
        const Common::Vec2 ball_line      = *targetBallPlacement - ball.position;
        const int          ball_obs_count = std::ceil(ball_line.length() / (ballAreaRadius + current_robot_radius));

        for (int i = 0; i < ball_obs_count; i++)
        {
            const float        t        = (float) i / (float) ball_obs_count;
            const Common::Vec2 ball_obs = ball.position + ball_line * t;
            obs_map.addCircle({ball_obs, ballAreaRadius + current_robot_radius});
        }
    }
}

void Ai::ERRTSetGkClearObstacles(int robot_num)
{
    obs_map.resetMap();

    // our penalty area
    static constexpr float area_extension_size     = 200.0f;
    const float            penalty_area_half_width = penalty_area_width / 2.0f;

    const Common::Vec2 start{side * field_width, -(penalty_area_half_width + area_extension_size)};

    const float w = -side * (area_extension_size + penalty_area_r);
    const float h = penalty_area_width + 2 * area_extension_size;

    obs_map.addRectangle({start, w, h});
}
} // namespace Tyr::Soccer