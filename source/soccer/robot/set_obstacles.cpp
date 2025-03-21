#include "robot.h"

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
    const float extension_factor = std::max(0.0f, (state.velocity.length() - 1000.0f) / 9000.0f);
    return Common::field().robot_radius * (1.0f + extension_factor);
}

static float calculateOtherRadius(const Common::RobotState &t_current, const Common::RobotState &t_other)
{
    static constexpr float kMaxExtension             = 150.0f;
    static constexpr float kSpeedToReachMaxExtension = 1500.0f;

    const Common::Vec2 velocity_diff           = t_other.velocity - t_current.velocity;
    const Common::Vec2 connection_vector       = (t_other.position - t_current.position).normalized();
    const float        velocity_dot_connection = velocity_diff.dot(connection_vector) - 300.0f;

    const float extension =
        std::clamp(kMaxExtension * velocity_dot_connection / kSpeedToReachMaxExtension, 0.0f, kMaxExtension);
    return Common::field().robot_radius + extension;
}

void Robot::setObstacles(const NavigationFlags t_flags)
{
    const bool dont_extend = !!(t_flags & NavigationFlags::ForceNoExtraMargin);

    const bool ourPenalty = !(t_flags & NavigationFlags::ForceNoOwnPenaltyArea) && !State::ref().ourBallPlacement();
    const bool oppPenalty = !State::ref().ballPlacement();

    const bool oppPenaltyBig = State::ref().freeKick() || State::ref().stop();

    const float current_robot_radius = dont_extend
        ? Common::field().robot_radius
        : calculateRobotRadius(state());

    m_obs_map.resetMap();

    if (!!(t_flags & NavigationFlags::ForceNoObstacles))
    {
        Common::logWarning("Robot {} is navigating with no obstacles", state().vision_id);
        return;
    }

    // own
    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        const Common::RobotState& own = State::world().own_robot[i];

        if ((own.seen_state != Common::SeenState::CompletelyOut) && (i != state().vision_id) &&
            (own.vision_id != state().vision_id))
        {
            const float radius = calculateRobotRadius(state());
            m_obs_map.add({own.position, current_robot_radius + radius}, Physicality::Physical);
        }
    }

    // opp
    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        const Common::RobotState& opp = State::world().opp_robot[i];
        if (opp.seen_state != Common::SeenState::CompletelyOut)
        {
            const float radius = dont_extend ? Common::field().robot_radius
                                                         : calculateOtherRadius(state(),
                                                                                opp);
            m_obs_map.add({opp.position, radius + current_robot_radius}, Physicality::Physical);
        }
    }

    float ball_radius = 0.0f;
    if (!!(t_flags & NavigationFlags::ForceBallObstacle) || !State::ref().allowedNearBall())
        ball_radius = ballAreaRadius;
    else if (!!(t_flags & NavigationFlags::ForceBallMediumObstacle))
        ball_radius = 230.0f;
    else if (!!(t_flags & NavigationFlags::ForceBallSmallObstacle))
        ball_radius = 60.0f;

    if (ball_radius > 0.0f)
    {
        // the ball itself
        m_obs_map.add({State::world().ball.position, Common::field().ball_radius + current_robot_radius}, Physicality::Physical);

        // the area around the ball
        m_obs_map.add({State::world().ball.position, ball_radius + current_robot_radius}, Physicality::Virtual);
    }

    // goal posts
    static constexpr float kWallThickness = 20.0f;
    const Common::Vec2 goal_post_size {
        Common::field().goal_depth + Common::field().robot_radius * 2.f,
        kWallThickness + Common::field().robot_radius * 2.0f
    };

    const Common::Rect goal_post_1 {
            {
                -Common::field().width - (goal_post_size.x - Common::field().robot_radius),
                -Common::field().goal_width / 2.0f - (goal_post_size.y - Common::field().robot_radius)
            },
        goal_post_size.x,
        goal_post_size.y
    };

    const Common::Rect goal_post_2 {
            {
                -Common::field().width - (goal_post_size.x - Common::field().robot_radius),
                Common::field().goal_width / 2.0f - (goal_post_size.y - Common::field().robot_radius)
            },
        goal_post_size.x,
        goal_post_size.y
    };

    const Common::Rect goal_post_3 {
            {
                Common::field().width - Common::field().robot_radius,
                -Common::field().goal_width / 2.0f - (goal_post_size.y - Common::field().robot_radius)
            },
        goal_post_size.x,
        goal_post_size.y
    };

    const Common::Rect goal_post_4 {
            {
                Common::field().width - Common::field().robot_radius,
                Common::field().goal_width / 2.0f - (goal_post_size.y - Common::field().robot_radius)
            },
        goal_post_size.x,
        goal_post_size.y
    };
    
    m_obs_map.add(goal_post_1, Physicality::Physical);
    m_obs_map.add(goal_post_2, Physicality::Physical);
    m_obs_map.add(goal_post_3, Physicality::Physical);
    m_obs_map.add(goal_post_4, Physicality::Physical);

    const float penalty_area_half_width = Common::field().penalty_area_width / 2.0f;

    if (ourPenalty)
    {
        const Common::Vec2 start{State::side() * (Common::field().width + penaltyAreaExtensionBehindGoal),
                                 -(penalty_area_half_width + current_robot_radius)};

        const float w =
            -State::side() * (penaltyAreaExtensionBehindGoal + current_robot_radius + Common::field().penalty_area_depth);
        const float h = Common::field().penalty_area_width + 2 * current_robot_radius;

        m_obs_map.add({start, w, h}, Physicality::Virtual);
    }

    if (oppPenalty)
    {
        const Common::Vec2 start{-State::side() * (Common::field().width + penaltyAreaExtensionBehindGoal),
                                 -(penalty_area_half_width + current_robot_radius)};

        const float w =
            State::side() * (penaltyAreaExtensionBehindGoal + current_robot_radius + Common::field().penalty_area_depth);
        const float h = Common::field().penalty_area_width + 2 * current_robot_radius;

        m_obs_map.add({start, w, h}, Physicality::Virtual);
    }

    if (oppPenaltyBig)
    {
        const float big_penalty_area_r          = Common::field().penalty_area_depth + bigPenaltyAddition;
        const float big_penalty_area_w          = Common::field().penalty_area_width + 2.0f * bigPenaltyAddition;
        const float big_penalty_area_half_width = big_penalty_area_w / 2.0f;

        const Common::Vec2 start{-State::side() * (Common::field().width + penaltyAreaExtensionBehindGoal),
                                 -(big_penalty_area_half_width + current_robot_radius)};

        const float w = State::side() * (penaltyAreaExtensionBehindGoal + current_robot_radius + big_penalty_area_r);
        const float h = big_penalty_area_w + 2 * current_robot_radius;

        m_obs_map.add({start, w, h}, Physicality::Virtual);
    }

    // avoid the line between the ball and the placement point
    if (State::ref().theirBallPlacement())
    {
        const float placement_clearance = ballAreaRadius * 2.0f;
        const Common::Vec2 ball_line      = State::ref().designated_position - State::world().ball.position;
        const int          ball_obs_count = std::ceil(ball_line.length() / (0.5f * placement_clearance + current_robot_radius));

        for (int i = 0; i < ball_obs_count; i++)
        {
            const float        t        = static_cast<float>(i) / static_cast<float>(ball_obs_count);
            const Common::Vec2 ball_obs = State::world().ball.position + ball_line * t;
            m_obs_map.add({ball_obs, placement_clearance + current_robot_radius}, Physicality::Virtual);
        }
    }
}
} // namespace Tyr::Soccer
