#include "../../ai.h"

namespace Tyr::Soccer
{
// This is 500 mm in the rules, but we add some extra to avoid
static constexpr float ballAreaRadius = 550.0f;

// We allow errt points to be 250 mm outside the field,
// so set this to some higher value
static constexpr float penaltyAreaExtensionBehindGoal = 300.0f;

// This is 200 mm in the rules, but we add some extra to avoid
static constexpr float bigPenaltyAddition = 220.0f;

static float calculateRobotRadius(const Common::RobotState &state)
{
    const float extension_factor = std::min(1.0f, state.velocity.length() / 10000.0f);
    return Common::field().robot_radius * (1.0f + extension_factor);
}

static float calculateOtherRadius(const Common::RobotState &t_current, const Common::RobotState &t_other)
{
    static constexpr float kMaxExtension             = 150.0f;
    static constexpr float kSpeedToReachMaxExtension = 1500.0f;

    const Common::Vec2 velocity_diff = t_current.velocity - t_other.velocity;
    const float        extension =
        std::clamp(velocity_diff.length() * kMaxExtension / kSpeedToReachMaxExtension, 0.0f, kMaxExtension);
    return Common::field().robot_radius + extension;
}

void Ai::setObstacles(const int t_robot_num, const NavigationFlags t_flags)
{
    ObstacleMap& map = m_obsMap[t_robot_num];

    const bool ourPenalty = t_robot_num != m_gk && !m_ref_state.ourBallPlacement();
    const bool oppPenalty = !m_ref_state.ballPlacement();

    const bool oppPenaltyBig = m_ref_state.freeKick() || m_ref_state.stop();

    const float current_robot_radius = calculateRobotRadius(m_own_robot[t_robot_num].state());

    map.resetMap();

    if (t_flags & NavigationFlagsForceNoObstacles)
    {
        Common::logWarning("Robot {} is navigating with no obstacles", t_robot_num);
        return;
    }

    // own
    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        if ((m_own_robot[i].state().seen_state != Common::SeenState::CompletelyOut) && (i != t_robot_num) &&
            (m_own_robot[i].state().vision_id != m_own_robot[t_robot_num].state().vision_id))
        {
            const float radius = calculateRobotRadius(m_own_robot[t_robot_num].state());
            map.add({m_own_robot[i].state().position, current_robot_radius + radius});
        }
    }

    // opp
    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        if (m_world_state.opp_robot[i].seen_state != Common::SeenState::CompletelyOut)
        {
            // Don't extend opp robots if we're the attacker
            const float radius = t_robot_num == m_attack ? Common::field().robot_radius
                                                         : calculateOtherRadius(m_own_robot[t_robot_num].state(),
                                                                                m_world_state.opp_robot[i]);

            map.add({m_world_state.opp_robot[i].position, radius + current_robot_radius});
        }
    }

    float ball_radius = 0.0f;
    if ((t_flags & NavigationFlagsForceBallObstacle) || !m_ref_state.allowedNearBall())
        ball_radius = ballAreaRadius;
    else if (m_ref_state.ourBallPlacement() && t_robot_num != m_attack && t_robot_num != m_mid5)
        ball_radius = ballAreaRadius;
    else if (t_flags & NavigationFlagsForceBallMediumObstacle)
        ball_radius = 230.0f;
    else if (t_flags & NavigationFlagsForceBallSmallObstacle)
        ball_radius = 60.0f;

    if (ball_radius > 0.0f)
    {
        map.add({m_world_state.ball.position, ball_radius + current_robot_radius});
    }

    const float penalty_area_half_width = Common::field().penalty_area_width / 2.0f;

    if (ourPenalty)
    {
        const Common::Vec2 start{m_side * (Common::field().width + penaltyAreaExtensionBehindGoal),
                                 -(penalty_area_half_width + current_robot_radius)};

        const float w =
            -m_side * (penaltyAreaExtensionBehindGoal + current_robot_radius + Common::field().penalty_area_depth);
        const float h = Common::field().penalty_area_width + 2 * current_robot_radius;

        map.add({start, w, h});
    }

    if (oppPenalty)
    {
        const Common::Vec2 start{-m_side * (Common::field().width + penaltyAreaExtensionBehindGoal),
                                 -(penalty_area_half_width + current_robot_radius)};

        const float w =
            m_side * (penaltyAreaExtensionBehindGoal + current_robot_radius + Common::field().penalty_area_depth);
        const float h = Common::field().penalty_area_width + 2 * current_robot_radius;

        map.add({start, w, h});
    }

    if (oppPenaltyBig)
    {
        const float big_penalty_area_r          = Common::field().penalty_area_depth + bigPenaltyAddition;
        const float big_penalty_area_w          = Common::field().penalty_area_width + 2.0f * bigPenaltyAddition;
        const float big_penalty_area_half_width = big_penalty_area_w / 2.0f;

        const Common::Vec2 start{-m_side * (Common::field().width + penaltyAreaExtensionBehindGoal),
                                 -(big_penalty_area_half_width + current_robot_radius)};

        const float w = m_side * (penaltyAreaExtensionBehindGoal + current_robot_radius + big_penalty_area_r);
        const float h = big_penalty_area_w + 2 * current_robot_radius;

        map.add({start, w, h});
    }

    // avoid the line between the ball and the placement point
    if (m_ref_state.theirBallPlacement())
    {
        const Common::Vec2 ball_line      = m_ref_state.designated_position - m_world_state.ball.position;
        const int          ball_obs_count = std::ceil(ball_line.length() / (ballAreaRadius + current_robot_radius));

        for (int i = 0; i < ball_obs_count; i++)
        {
            const float        t        = static_cast<float>(i) / static_cast<float>(ball_obs_count);
            const Common::Vec2 ball_obs = m_world_state.ball.position + ball_line * t;
            map.add({ball_obs, ballAreaRadius + current_robot_radius});
        }
    }
}
} // namespace Tyr::Soccer
