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
    const float extension_factor = std::min(1.0f, state.velocity.length() / 1000.0f);
    return robotRadius * (1.0f + extension_factor);
}

void Ai::ERRTSetObstacles(int robot_num, bool bll, bool field)
{
    const bool ourPenalty = field || (robot_num != gk && !REF_playState->ourPlaceBall());
    const bool oppPenalty = field || !REF_playState->ourPlaceBall();

    const bool oppPenaltyBig = REF_playState->freeKick() || REF_playState->stop();

    const float current_robot_radius = calculateRobotRadius(OwnRobot[robot_num].State);

    clear_map();

    // own
    for (int i = 0; i < Common::Setting::kMaxOnFieldTeamRobots; i++)
    {
        if ((OwnRobot[i].State.seenState != Common::CompletelyOut) && (i != robot_num) &&
            (OwnRobot[i].State.vision_id != OwnRobot[robot_num].State.vision_id))
        {
            AddCircle({OwnRobot[i].State.Position, current_robot_radius + robotRadius});
            // Common::debug().drawCircle(OwnRobot[i].State.Position,ownRobotRadius + (!dribble)*ownRobotRadius,Cyan);
        }
    }

    // opp
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (OppRobot[i].seenState != Common::CompletelyOut)
        {
            const float radius = calculateRobotRadius(OppRobot[i]);

            AddCircle({OppRobot[i].Position, radius + current_robot_radius});
            // Common::debug().drawCircle(OppRobot[i].Position,ownRobotRadius + (!dribble)*ownRobotRadius,Cyan);
        }
    }

    if (bll || !REF_playState->allowedNearBall())
    {
        AddCircle({ball.Position, ballAreaRadius + current_robot_radius});
    }

    const float penalty_area_half_width = penalty_area_width / 2.0f;

    if (ourPenalty)
    {
        AddRectangle(side * (field_width + penaltyAreaExtensionBehindGoal),
                     -(penalty_area_half_width + current_robot_radius),
                     -side * (penaltyAreaExtensionBehindGoal + current_robot_radius + penalty_area_r),
                     penalty_area_width + 2 * current_robot_radius);
    }

    if (oppPenalty)
    {
        AddRectangle(-side * (field_width + penaltyAreaExtensionBehindGoal),
                     -(penalty_area_half_width + current_robot_radius),
                     side * (penaltyAreaExtensionBehindGoal + current_robot_radius + penalty_area_r),
                     penalty_area_width + 2 * current_robot_radius);
    }

    if (oppPenaltyBig)
    {
        const float big_penalty_area_r      = penalty_area_r + bigPenaltyAddition;
        const float big_penalty_area_w      = penalty_area_width + bigPenaltyAddition;
        const float penalty_area_half_width = big_penalty_area_w / 2.0f;

        AddRectangle(-side * (field_width + penaltyAreaExtensionBehindGoal),
                     -(penalty_area_half_width + current_robot_radius),
                     side * (penaltyAreaExtensionBehindGoal + current_robot_radius + big_penalty_area_r),
                     big_penalty_area_w + 2 * current_robot_radius);
        //		Common::debug().drawRect( Common::Vec2(-side*(field_width+185.0f) , -penalty_circle_center_y - 200) ,
        // side*(385.0f+big_penalty_area_r) , penalty_area_width + 400,Purple );
    }

    // avoid the line between the ball and the placement point
    if (REF_playState->theirPlaceBall())
    {
        const Common::Vec2 ball_line      = *targetBallPlacement - ball.Position;
        const int          ball_obs_count = std::ceil(ball_line.length() / (ballAreaRadius + current_robot_radius));

        for (int i = 0; i < ball_obs_count; i++)
        {
            const float        t        = (float) i / (float) ball_obs_count;
            const Common::Vec2 ball_obs = ball.Position + ball_line * t;
            AddCircle({ball_obs, ballAreaRadius + current_robot_radius});
        }
    }
}

void Ai::ERRTSetGkClearObstacles(int robot_num)
{
    clear_map();

    // our penalty area
    static constexpr float area_extension_size     = 200.0f;
    const float            penalty_area_half_width = penalty_area_width / 2.0f;
    AddRectangle(side * field_width, -(penalty_area_half_width + area_extension_size),
                 -side * (area_extension_size + penalty_area_r), penalty_area_width + 2 * area_extension_size);
}
} // namespace Tyr::Soccer