#include "../ai.h"

#include "../tactics/circle_ball.h"
#include "../tactics/def.h"
#include "../tactics/gk.h"

#include "../skills/wait_for_ball.h"

namespace Tyr::Soccer
{

void Ai::calcMinBallWallDist(double &t_min_dist, Common::Vec2 &t_closest_point)
{
    double       min_distance          = std::numeric_limits<double>::max();
    int          min_distance_wall_idx = -1;
    Common::Vec2 closest_point;

    Common::Vec2 fieldTopLeft     = Common::Vec2(-Common::field().width - Common::field().boundary_width,
                                                 Common::field().height + Common::field().boundary_width);
    Common::Vec2 fieldTopRight    = Common::Vec2(Common::field().width + Common::field().boundary_width,
                                                 Common::field().height + Common::field().boundary_width);
    Common::Vec2 fieldBottomLeft  = Common::Vec2(-Common::field().width - Common::field().boundary_width,
                                                 -Common::field().height - Common::field().boundary_width);
    Common::Vec2 fieldBottomRight = Common::Vec2(Common::field().width + Common::field().boundary_width,
                                                 -Common::field().height - Common::field().boundary_width);

    Common::Vec2 leftGoalTopLeft =
        Common::Vec2(-Common::field().width - Common::field().boundary_width, Common::field().goal_width / 2.f);
    Common::Vec2 leftGoalTopRight = Common::Vec2(-Common::field().width, Common::field().goal_width / 2.f);
    Common::Vec2 leftGoalBottomLeft =
        Common::Vec2(-Common::field().width - Common::field().boundary_width, -Common::field().goal_width / 2.f);
    Common::Vec2 leftGoalBottomRight = Common::Vec2(-Common::field().width, -Common::field().goal_width / 2.f);

    Common::Vec2 rightGoalTopLeft = Common::Vec2(Common::field().width, Common::field().goal_width / 2.f);
    Common::Vec2 rightGoalTopRight =
        Common::Vec2(Common::field().width + Common::field().boundary_width, Common::field().goal_width / 2.f);
    Common::Vec2 rightGoalBottomLeft = Common::Vec2(Common::field().width, -Common::field().goal_width / 2.f);
    Common::Vec2 rightGoalBottomRight =
        Common::Vec2(Common::field().width + Common::field().boundary_width, -Common::field().goal_width / 2.f);

    std::vector<Common::LineSegment> walls;
    // field walls
    walls.push_back(Common::LineSegment(fieldTopLeft, fieldTopRight));
    walls.push_back(Common::LineSegment(fieldBottomLeft, fieldBottomRight));
    walls.push_back(Common::LineSegment(fieldTopLeft, fieldBottomLeft));
    walls.push_back(Common::LineSegment(fieldTopRight, fieldBottomRight));

    // goal walls
    walls.push_back(Common::LineSegment(leftGoalTopLeft, leftGoalTopRight));
    walls.push_back(Common::LineSegment(leftGoalBottomLeft, leftGoalBottomRight));
    walls.push_back(Common::LineSegment(rightGoalTopLeft, rightGoalTopRight));
    walls.push_back(Common::LineSegment(rightGoalBottomLeft, rightGoalBottomRight));

    for (auto i = 0; i < walls.size(); i++)
    {
        double distance = walls[i].distanceTo(m_world_state.ball.position);
        if (distance < min_distance)
        {
            min_distance          = distance;
            min_distance_wall_idx = i;
            closest_point         = walls[i].closestPoint(m_world_state.ball.position);
        }
    }

    Common::logError("min_distance: {}", min_distance);
    Common::debug().draw(walls[min_distance_wall_idx], Common::Color::red(), 30.f);
    Common::debug().draw(closest_point, Common::Color::blue(), 30.f);

    t_min_dist      = min_distance;
    t_closest_point = closest_point;
}

void Ai::switchBallPlacementStateDelayed(int t_wait_frames, OurBallPlacementState t_new_state)
{
    m_our_ball_placement_state_wait_frames++;

    if (m_our_ball_placement_state_wait_frames >= t_wait_frames)
    {
        if (placeBallLost())
        {
            m_our_ball_placement_state = OurBallPlacementState::Idle;
        }
        else
        {
            m_our_ball_placement_state = t_new_state;
        }
    }
    m_our_ball_placement_state_wait_frames++;
}

void Ai::resetBallPlacementStateFrameCounter()
{
    m_our_ball_placement_state_wait_frames = 0;
}

void Ai::placeBallLongDistance()
{
    const Common::Vec2 receiver_pos =
        m_ref_state.designated_position +
        (m_ref_state.designated_position - m_world_state.ball.position).normalized() * Common::field().robot_radius;

    WaitForBallSkill{receiver_pos}.execute(m_own_robot[m_mid5]);

    if (m_own_robot[m_mid5].state().position.distanceTo(receiver_pos) < 100)
    {
        CircleBallTactic{(m_world_state.ball.position - receiver_pos).toAngle(), 4000, 0}.execute(
            m_own_robot[m_attack]);
    }
    else
    {
        CircleBallTactic{(m_world_state.ball.position - receiver_pos).toAngle(), 0, 0}.execute(m_own_robot[m_attack]);
    }
    if (m_world_state.ball.velocity.length() < 100.0f &&
        m_ref_state.designated_position.distanceTo(m_world_state.ball.position) < 1000)
    {
        m_our_ball_placement_state = OurBallPlacementState::Idle;
    }
}

bool Ai::placeBallLost()
{
    return ((m_own_robot[m_attack].state().position + m_own_robot[m_mid5].state().position) / 2)
                   .distanceTo(m_world_state.ball.position) > 250.0f &&
           m_world_state.ball.seen_state == Common::SeenState::Seen;
}

void Ai::generateKissPoints(double t_distance, Common::Vec2 &t_pos1, Common::Vec2 &t_pos2)
{

    t_pos1 = m_world_state.ball.position +
             (m_world_state.ball.position - m_ref_state.designated_position).normalized() * t_distance;
    t_pos2 = m_world_state.ball.position +
             (m_ref_state.designated_position - m_world_state.ball.position).normalized() * t_distance;
}

void Ai::placeBall()
{
    GkTactic{}.execute(m_own_robot[m_gk]);

    DefTactic{1}.execute(m_own_robot[m_def1]);
    DefTactic{2}.execute(m_own_robot[m_def2]);

    int zone_idx = 0;
    for (const auto &mid : m_prioritized_mids)
    {
        if (mid == &m_mid5)
            continue;
        m_own_robot[*mid].face(m_world_state.ball.position);
        m_own_robot[*mid].navigate(m_sorted_zones[zone_idx]->best_pos, VelocityProfile::aroom(),
                                   NavigationFlags::BallPlacementLine);
        ++zone_idx;
    }

    double wall_distance_threshold = 150.f;
    double angle_offset            = 60.f;

    double       min_wall_distance;
    Common::Vec2 closest_wall_point;
    calcMinBallWallDist(min_wall_distance, closest_wall_point);

    Common::logDebug("state: {}", static_cast<int>(m_our_ball_placement_state));

    auto final_ball_pos = m_ref_state.designated_position;

    const double desired_distance = 100.0f;

    const Common::Rect our_goal_area(
        Common::Vec2(-Common::field().width - Common::field().boundary_width, Common::field().goal_width / 2.f),
        Common::Vec2(-Common::field().width, -Common::field().goal_width / 2.f));

    const Common::Rect opp_goal_area(
        Common::Vec2(Common::field().width, Common::field().goal_width / 2.f),
        Common::Vec2(Common::field().width + Common::field().boundary_width, -Common::field().goal_width / 2.f));

    const bool ball_in_goal =
        our_goal_area.inside(m_world_state.ball.position) || opp_goal_area.inside(m_world_state.ball.position);

    const Common::Rect field_access_area(
        Common::Vec2(-Common::field().width - Common::field().boundary_width + Common::field().robot_radius,
                     -Common::field().height - Common::field().boundary_width + Common::field().robot_radius),
        Common::Vec2(Common::field().width + Common::field().boundary_width - Common::field().robot_radius,
                     Common::field().height + Common::field().boundary_width - Common::field().robot_radius));

    // const Common::Vec2 robots_mid_point = (m_own_robot[m_attack].state().position +
    // m_own_robot[m_mid5].state().position) / 2;

    switch (m_our_ball_placement_state)
    {
    case OurBallPlacementState::Idle:
        if (m_world_state.ball.velocity.length() > 100.0f && m_world_state.ball.seen_state == Common::SeenState::Seen)
        {
            m_our_ball_placement_state = OurBallPlacementState::Idle;
        }
        else if (min_wall_distance < wall_distance_threshold && !ball_in_goal)
        {
            m_our_ball_placement_state = OurBallPlacementState::Stuck;
        }
        else if (m_world_state.ball.position.distanceTo(final_ball_pos) > 5000.0f)
        {
            m_our_ball_placement_state = OurBallPlacementState::LongDistance;
        }
        else if (m_world_state.ball.position.distanceTo(final_ball_pos) > desired_distance)
        {
            m_our_ball_placement_state = OurBallPlacementState::KissInit;
        }
        else
        {
            m_our_ball_placement_state = OurBallPlacementState::Done;
        }
        break;
    case OurBallPlacementState::KissInit:
        if (m_world_state.ball.velocity.length() < 100.0f)
        {
            Common::Vec2 attack_pos, mid5_pos;
            generateKissPoints(250.0f, attack_pos, mid5_pos);
            m_own_robot[m_attack].face(m_world_state.ball.position);
            m_own_robot[m_mid5].face(m_world_state.ball.position);

            if (!field_access_area.inside(attack_pos) || !field_access_area.inside(mid5_pos))
            {
                m_our_ball_placement_state       = OurBallPlacementState::Stuck;
                m_our_ball_placement_force_stuck = true;
                break;
            }
            m_own_robot[m_attack].navigate(attack_pos, VelocityProfile::aroom(),
                                           NavigationFlags::BallSmallObstacle);
            m_own_robot[m_mid5].navigate(mid5_pos, VelocityProfile::aroom(), NavigationFlags::BallSmallObstacle);

            if (m_own_robot[m_attack].state().position.distanceTo(attack_pos) < 20.0f &&
                m_own_robot[m_mid5].state().position.distanceTo(mid5_pos) < 20.0f)
            {
                switchBallPlacementStateDelayed(20, OurBallPlacementState::Kissing);
            }
            else
            {
                resetBallPlacementStateFrameCounter();
            }
        }
        else
        {
            m_our_ball_placement_state = OurBallPlacementState::Idle;
        }
        break;
    case OurBallPlacementState::KissTouch:
        if (m_world_state.ball.velocity.length() < 100.0f)
        {
            Common::Vec2 attack_pos, mid5_pos;
            generateKissPoints(75.0f, attack_pos, mid5_pos);
            m_own_robot[m_attack].face(m_world_state.ball.position);
            m_own_robot[m_mid5].face(m_world_state.ball.position);
            m_own_robot[m_attack].navigate(attack_pos, VelocityProfile::sooski(), NavigationFlags::NoObstacles);
            m_own_robot[m_mid5].navigate(mid5_pos, VelocityProfile::sooski(), NavigationFlags::NoObstacles);

            if (m_own_robot[m_attack].state().position.distanceTo(attack_pos) < 20.0f &&
                m_own_robot[m_mid5].state().position.distanceTo(mid5_pos) < 20.0f)
            {
                switchBallPlacementStateDelayed(20, OurBallPlacementState::Kissing);
            }
            else
            {
                resetBallPlacementStateFrameCounter();
            }
        }
        else
        {
            m_our_ball_placement_state = OurBallPlacementState::Idle;
        }
        break;
    case OurBallPlacementState::Kissing:
        if (!placeBallLost())
        {
            const Common::Vec2 attack_pos =
                final_ball_pos + (m_own_robot[m_attack].state().position - final_ball_pos).normalized() * 75.0f;
            const Common::Vec2 mid5_pos =
                final_ball_pos + (final_ball_pos - m_own_robot[m_attack].state().position).normalized() * 75.0f;

            m_own_robot[m_attack].navigate(attack_pos, VelocityProfile::sooski(), NavigationFlags::NoObstacles);
            m_own_robot[m_mid5].navigate(mid5_pos, VelocityProfile::sooski(), NavigationFlags::NoObstacles);

            Common::logDebug("frame step wait: {}", m_our_ball_placement_state_wait_frames);
            if (m_own_robot[m_attack].state().position.distanceTo(attack_pos) < 20.0f &&
                m_own_robot[m_mid5].state().position.distanceTo(mid5_pos) < 20.0f)
            {
                m_our_ball_placement_attack_final_pos =
                    m_ref_state.designated_position +
                    (m_own_robot[m_attack].state().position - m_ref_state.designated_position).normalized() * 250.0f;
                m_our_ball_placement_mid5_final_pos =
                    m_ref_state.designated_position +
                    (m_own_robot[m_mid5].state().position - m_ref_state.designated_position).normalized() * 250.0f;
                switchBallPlacementStateDelayed(20, OurBallPlacementState::KissingDone);
            }
            else
            {
                resetBallPlacementStateFrameCounter();
            }
        }
        else
        {
            m_our_ball_placement_state = OurBallPlacementState::Idle;
        }
        break;
    case OurBallPlacementState::Stuck:

        if (m_our_ball_placement_stuck_count >= 100)
        {
            m_our_ball_placement_stuck_count = 100;
            const Common::Vec2 attack_pos =
                m_world_state.ball.position + (m_world_state.ball.position - closest_wall_point).normalized() * 1000.0f;
            m_own_robot[m_attack].face(m_world_state.ball.position);
            m_own_robot[m_attack].navigate(attack_pos, VelocityProfile::mamooli(), NavigationFlags::NoObstacles);
        }
        else
        {
            CircleBallTactic{(m_world_state.ball.position - closest_wall_point).toAngle() +
                                 Common::Angle::fromDeg(angle_offset),
                             3000, 0}
                .execute(m_own_robot[m_attack]);
        }

        if (m_own_robot[m_attack].state().position.distanceTo(m_world_state.ball.position) > 400.0f)
        {
            m_our_ball_placement_stuck_count = 0;
        }

        if (m_world_state.ball.velocity.length() > 20.0f)
        {
            m_our_ball_placement_stuck_count = 0;
            m_our_ball_placement_force_stuck = false;
        }

        if (min_wall_distance >= wall_distance_threshold && !m_our_ball_placement_force_stuck)
        {
            m_our_ball_placement_state = OurBallPlacementState::Idle;
        }
        m_our_ball_placement_stuck_count++;
        break;
    case OurBallPlacementState::LongDistance:
        placeBallLongDistance();
        break;
    case OurBallPlacementState::KissingDone:
        m_own_robot[m_attack].face(m_ref_state.designated_position);
        m_own_robot[m_mid5].face(m_ref_state.designated_position);

        m_own_robot[m_attack].navigate(m_our_ball_placement_attack_final_pos, VelocityProfile::aroom(),
                                       NavigationFlags::BallSmallObstacle);
        m_own_robot[m_mid5].navigate(m_our_ball_placement_mid5_final_pos, VelocityProfile::aroom(),
                                     NavigationFlags::BallSmallObstacle);
        if (m_own_robot[m_attack].state().position.distanceTo(m_our_ball_placement_attack_final_pos) < 20.0f &&
            m_own_robot[m_mid5].state().position.distanceTo(m_our_ball_placement_mid5_final_pos) < 20.0f)
        {
            m_our_ball_placement_attack_final_pos =
                m_ref_state.designated_position +
                (m_own_robot[m_attack].state().position - m_ref_state.designated_position).normalized() * 800.0f;
            m_our_ball_placement_mid5_final_pos =
                m_ref_state.designated_position +
                (m_own_robot[m_mid5].state().position - m_ref_state.designated_position).normalized() * 600.0f;
            switchBallPlacementStateDelayed(20, OurBallPlacementState::Done);
        }
        else
        {
            resetBallPlacementStateFrameCounter();
        }

        break;

    case OurBallPlacementState::Done:
    {
        m_own_robot[m_attack].face(m_ref_state.designated_position);
        m_own_robot[m_mid5].face(m_ref_state.designated_position);

        m_own_robot[m_attack].navigate(m_our_ball_placement_attack_final_pos, VelocityProfile::aroom(),
                                       NavigationFlags::BallObstacle);
        m_own_robot[m_mid5].navigate(m_our_ball_placement_mid5_final_pos, VelocityProfile::aroom(),
                                     NavigationFlags::BallObstacle);

        if (m_own_robot[m_attack].state().position.distanceTo(m_our_ball_placement_attack_final_pos) < 20.0f &&
            m_own_robot[m_mid5].state().position.distanceTo(m_our_ball_placement_mid5_final_pos) < 20.0f)
        {
            switchBallPlacementStateDelayed(20, OurBallPlacementState::Idle);
        }
        else
        {
            resetBallPlacementStateFrameCounter();
        }
    }
    break;
    }
}
} // namespace Tyr::Soccer
