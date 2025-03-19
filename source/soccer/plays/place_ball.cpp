

#include "../ai.h"

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
    Common::Vec2 receiver_pos =
        m_ref_state.designated_position +
        (m_ref_state.designated_position - m_world_state.ball.position).normalized() * Common::field().robot_radius;
    m_own_robot[m_mid5].face(m_world_state.ball.position);
    if (m_world_state.ball.velocity.length() >= 100)
    {
        receiver_pos =
            Common::Line::fromTwoPoints(m_world_state.ball.position,
                                        m_world_state.ball.velocity.normalized() * 1000 + m_world_state.ball.position)
                .closestPoint(m_own_robot[m_mid5].state().position);
    }
    m_own_robot[m_mid5].navigate(receiver_pos, VelocityProfile::mamooli());
    if (m_own_robot[m_mid5].state().position.distanceTo(receiver_pos) < 100)
    {
        circleBall(m_attack, (m_world_state.ball.position - receiver_pos).toAngle(), 4000, 0);
    }
    else
    {
        circleBall(m_attack, (m_world_state.ball.position - receiver_pos).toAngle(), 0, 0);
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

void Ai::ourNewPlaceBall()
{
    gkHi(m_gk);
    defHi(m_def1, m_def2, nullptr);

    int zone_idx = 0;
    for (const auto &mid : m_prioritized_mids)
    {
        if (mid == &m_mid5)
            continue;
        m_own_robot[*mid].face(m_world_state.ball.position);
        m_own_robot[*mid].navigate(m_sorted_zones[zone_idx]->best_pos, VelocityProfile::aroom(), NavigationFlags::ForceBallObstacle);
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

    const Common::Rect our_goal_area(Common::Vec2(-Common::field().width - Common::field().boundary_width, Common::field().goal_width / 2.f),
                                     Common::Vec2(-Common::field().width, -Common::field().goal_width / 2.f));

    const Common::Rect opp_goal_area(Common::Vec2(Common::field().width, Common::field().goal_width / 2.f),
                                     Common::Vec2(Common::field().width + Common::field().boundary_width, -Common::field().goal_width / 2.f));

    const bool ball_in_goal = our_goal_area.inside(m_world_state.ball.position) || opp_goal_area.inside(m_world_state.ball.position);

    const Common::Rect field_access_area(
        Common::Vec2(-Common::field().width - Common::field().boundary_width + Common::field().robot_radius,
                     -Common::field().height - Common::field().boundary_width + Common::field().robot_radius),
        Common::Vec2(Common::field().width + Common::field().boundary_width - Common::field().robot_radius,
                     Common::field().height + Common::field().boundary_width - Common::field().robot_radius));

    // const Common::Vec2 robots_mid_point = (m_own_robot[m_attack].state().position + m_own_robot[m_mid5].state().position) / 2;

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
            m_own_robot[m_attack].navigate(attack_pos, VelocityProfile::aroom(), NavigationFlags::ForceBallSmallObstacle);
            m_own_robot[m_mid5].navigate(mid5_pos, VelocityProfile::aroom(), NavigationFlags::ForceBallSmallObstacle);

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
            m_own_robot[m_attack].navigate(attack_pos, VelocityProfile::sooski(), NavigationFlags::ForceNoObstacles);
            m_own_robot[m_mid5].navigate(mid5_pos, VelocityProfile::sooski(), NavigationFlags::ForceNoObstacles);

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

            m_own_robot[m_attack].navigate(attack_pos, VelocityProfile::sooski(), NavigationFlags::ForceNoObstacles);
            m_own_robot[m_mid5].navigate(mid5_pos, VelocityProfile::sooski(), NavigationFlags::ForceNoObstacles);

            Common::logDebug("frame step wait: {}", m_our_ball_placement_state_wait_frames);
            if (m_own_robot[m_attack].state().position.distanceTo(attack_pos) < 20.0f &&
                m_own_robot[m_mid5].state().position.distanceTo(mid5_pos) < 20.0f)
            {
            m_our_ball_placement_attack_final_pos = m_ref_state.designated_position + (m_own_robot[m_attack].state().position - m_ref_state.designated_position).normalized() * 250.0f;
            m_our_ball_placement_mid5_final_pos = m_ref_state.designated_position + (m_own_robot[m_mid5].state().position - m_ref_state.designated_position).normalized() * 250.0f;
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
            const Common::Vec2 attack_pos = m_world_state.ball.position + (m_world_state.ball.position - closest_wall_point).normalized() * 1000.0f;
            m_own_robot[m_attack].face(m_world_state.ball.position);
            m_own_robot[m_attack].navigate(attack_pos, VelocityProfile::mamooli(), NavigationFlags::ForceNoObstacles);
        } else {
            circleBall(m_attack,
                       (m_world_state.ball.position - closest_wall_point).toAngle() + Common::Angle::fromDeg(angle_offset),
                       3000, 0);
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

        m_own_robot[m_attack].navigate(m_our_ball_placement_attack_final_pos, VelocityProfile::aroom(), NavigationFlags::ForceBallSmallObstacle);
        m_own_robot[m_mid5].navigate(m_our_ball_placement_mid5_final_pos, VelocityProfile::aroom(), NavigationFlags::ForceBallSmallObstacle);
        if (m_own_robot[m_attack].state().position.distanceTo(m_our_ball_placement_attack_final_pos) < 20.0f &&
                m_own_robot[m_mid5].state().position.distanceTo(m_our_ball_placement_mid5_final_pos) < 20.0f)
            {
            m_our_ball_placement_attack_final_pos = m_ref_state.designated_position + (m_own_robot[m_attack].state().position - m_ref_state.designated_position).normalized() * 800.0f;
            m_our_ball_placement_mid5_final_pos = m_ref_state.designated_position + (m_own_robot[m_mid5].state().position - m_ref_state.designated_position).normalized() * 600.0f;
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

            m_own_robot[m_attack].navigate(m_our_ball_placement_attack_final_pos, VelocityProfile::aroom(), NavigationFlags::ForceBallObstacle);
            m_own_robot[m_mid5].navigate(m_our_ball_placement_mid5_final_pos, VelocityProfile::aroom(), NavigationFlags::ForceBallObstacle);

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

void Ai::ourPlaceBall()
{
    calcIsDefending();

#if 0
    m_assignments.clear();
    createGkAssignment();
    createDefAssignments();
    createMidAssignments();
    createAttackAssignment();
    assignRoles();
#endif

    gkHi(m_gk);
    defHi(m_def1, m_def2, nullptr);

    int zone_idx = 0;
    for (const auto &mid : m_prioritized_mids)
    {
        if (mid == &m_mid5)
            continue;

        m_own_robot[*mid].face(m_world_state.ball.position);
        m_own_robot[*mid].navigate(m_sorted_zones[zone_idx]->best_pos, VelocityProfile::aroom(), NavigationFlags::ForceBallObstacle);
        ++zone_idx;
    }

    static Common::Angle move_angle, temp_opp_ang;
    if (m_world_state.ball.position.distanceTo(m_ref_state.designated_position) > 100)
    {
        move_angle   = m_ref_state.designated_position.angleWith(m_world_state.ball.position);
        temp_opp_ang = move_angle + Common::Angle::fromDeg(180.0f);
    }

    static Common::Angle t_ang, t_opp_ang;
    static Common::Vec2  t_pos;
    static Common::Timer temp_time;

    static Common::Angle outFieldAng;
    static Common::Vec2  last_state_ball_pos;

    if (m_func_state == -2)
    {
        circleBall(m_attack, outFieldAng, 1000.0f, 0);

        // TODO: transition when m_dmf fits behind the ball
        if (last_state_ball_pos.distanceTo(m_world_state.ball.position) > 400)
        {
            m_func_count++;
        }
        if (m_func_count >= 20)
        {
            m_func_state = 3; // Back on track...
            m_func_count = 0;
            temp_time.start();
        }
    }
    else if (m_func_state == -1)
    {
        if (outFieldAng.deg() == 0.0f)
        {
            if (m_world_state.ball.position.x > 0)
            {
                if (m_world_state.ball.position.y > 0)
                {
                    if (m_world_state.ball.position.x > Common::field().width)
                        outFieldAng.setDeg(155.0f);
                    else // So std::fabs(m_world_state.ball.position.y) > Common::field().height
                        outFieldAng.setDeg(-65.0f);
                }
                else
                {
                    if (m_world_state.ball.position.x > Common::field().width)
                        outFieldAng.setDeg(-155.0f);
                    else // So std::fabs(m_world_state.ball.position.y) > Common::field().height
                        outFieldAng.setDeg(65.0f);
                }
            }
            else
            {
                if (m_world_state.ball.position.y > 0)
                {
                    if (std::fabs(m_world_state.ball.position.x) > Common::field().width)
                        outFieldAng.setDeg(25.0f);
                    else // So std::fabs(m_world_state.ball.position.y) > Common::field().height
                        outFieldAng.setDeg(-115.0f);
                }
                else
                {
                    if (std::fabs(m_world_state.ball.position.x) > Common::field().width)
                        outFieldAng.setDeg(-25.0f);
                    else // So std::fabs(m_world_state.ball.position.y)  > Common::field().height
                        outFieldAng.setDeg(115.0f);
                }
            }
        }
        circleBall(m_attack, outFieldAng, 0, 0);
        Common::logDebug("outFieldAng: {}", outFieldAng.deg());

        if (m_own_robot[m_attack].state().velocity.length() < 20)
        {
            m_func_count++;
        }
        if (m_func_count >= 10)
        {
            m_func_state        = -2;
            m_func_count        = 0;
            last_state_ball_pos = m_world_state.ball.position;
        }
    }
    else if (m_func_state == 0)
    {
        if (m_world_state.ball.position.distanceTo(m_ref_state.designated_position) < 95)
        {
            m_func_state = 7;
            m_func_count = 0;
            t_ang.setDeg(0.0f);
            t_opp_ang.setDeg(180.0f);
        }
        // TODO: only do this when m_dmf doesn't fit behind the ball
        else if (isOut(m_world_state.ball.position))
        { // Do a little shoot on the wall
            m_func_state = -1;
            m_func_count = 0;
            outFieldAng.setDeg(0.0f);
        }
        else if (m_world_state.ball.position.distanceTo(m_ref_state.designated_position) < 10000.0f)
        {
            m_func_state = 3;
            m_func_count = 0;
        }

        m_own_robot[m_mid5].target.angle = move_angle;

        m_own_robot[m_mid5].navigate(m_ref_state.designated_position.circleAroundPoint(temp_opp_ang, 250), VelocityProfile::aroom(),
                 NavigationFlags::ForceBallSmallObstacle);

        circleBall(m_attack, move_angle, 0, 0);
        Common::logDebug(":::{}", m_own_robot[m_attack].state().velocity.length());
        Common::logDebug("", m_own_robot[m_mid5].state().velocity.length());
        if (m_own_robot[m_attack].state().velocity.length() < 20 && m_own_robot[m_mid5].state().velocity.length() < 20)
        {
            m_func_count++;
        }
        if (m_func_count >= 10)
        {
            m_func_state        = 1;
            m_func_count        = 0;
            last_state_ball_pos = m_world_state.ball.position;
        }
    }
    else if (m_func_state == 1)
    {
        circleBall(m_attack, move_angle, 5000.0f, 0);
        waitForPass(m_mid5, false, &m_own_robot[m_attack].state().position);

        if (last_state_ball_pos.distanceTo(m_world_state.ball.position) > 400)
        { // TODO added this TEST IT with the kicker on (already tested without the kicker)
            m_func_count++;
        }
        if (m_own_robot[m_attack].state().position.distanceTo(m_world_state.ball.position) > 400)
        {
            m_func_count++;
        }
        if (m_func_count >= 20)
        {
            m_func_state = 2;
            m_func_count = 0;
            temp_time.start();
        }
    }
    else if (m_func_state == 2)
    {
        waitForPass(m_mid5, false, &m_own_robot[m_attack].state().position);

        if (temp_time.time().seconds() > 2)
        {
            m_func_count++;
        }
        if (m_func_count >= 10)
        {
            m_func_state = 3;
            m_func_count = 0;
            temp_time.start();
        }
    }
    else if (m_func_state == 3)
    {
        if (std::fabs((m_own_robot[m_attack].target.angle - move_angle).deg()) > 90)
            std::swap(move_angle, temp_opp_ang);
        m_own_robot[m_attack].target.angle = move_angle;
        m_own_robot[m_mid5].target.angle   = temp_opp_ang;

        m_own_robot[m_attack].navigate(m_world_state.ball.position.circleAroundPoint(temp_opp_ang, 250), VelocityProfile::aroom(),
                 NavigationFlags::ForceBallSmallObstacle);

        m_own_robot[m_mid5].navigate(m_world_state.ball.position.circleAroundPoint(move_angle, 250), VelocityProfile::aroom(),
                 NavigationFlags::ForceBallSmallObstacle);

        if (m_world_state.ball.position.circleAroundPoint(temp_opp_ang, 250)
                    .distanceTo(m_own_robot[m_attack].state().position) < 100 &&
            m_world_state.ball.position.circleAroundPoint(move_angle, 250)
                    .distanceTo(m_own_robot[m_mid5].state().position) < 100 &&
            m_world_state.ball.velocity.length() < 10)
        {

            m_func_count++;
        }
        if (m_func_count >= 10)
        {
            m_func_state = 4;
            m_func_count = 0;
            t_pos        = m_world_state.ball.position;
        }
    }
    else if (m_func_state == 4)
    {
        if (std::fabs((m_own_robot[m_attack].target.angle - move_angle).deg()) > 90)
            std::swap(move_angle, temp_opp_ang);

        m_own_robot[m_attack].target.angle = move_angle;
        m_own_robot[m_mid5].target.angle   = temp_opp_ang;
        m_own_robot[m_attack].navigate(t_pos.circleAroundPoint(temp_opp_ang, 75), VelocityProfile::sooski(),
                 NavigationFlags::ForceNoObstacles);

        m_own_robot[m_mid5].navigate(t_pos.circleAroundPoint(move_angle, 75), VelocityProfile::sooski(),
                 NavigationFlags::ForceNoObstacles);

        bool ball_has_slipt = ((m_own_robot[m_attack].state().position + m_own_robot[m_mid5].state().position) / 2)
                                  .distanceTo(m_world_state.ball.position) > 300;

        if (m_world_state.ball.seen_state == Common::SeenState::Seen && ball_has_slipt)
        {
            m_func_state = 3;
            m_func_count = 0;
        }

        if (t_pos.circleAroundPoint(temp_opp_ang, 75).distanceTo(m_own_robot[m_attack].state().position) < 40 &&
            t_pos.circleAroundPoint(move_angle, 75).distanceTo(m_own_robot[m_mid5].state().position) < 40)
        {
            m_func_count++;
        }
        if (m_func_count >= 10)
        {
            m_func_state = 5;
            m_func_count = 0;
            t_ang        = move_angle;
            t_opp_ang    = temp_opp_ang;
        }
    }
    else if (m_func_state == 5)
    {
        m_own_robot[m_attack].target.angle = t_ang;
        m_own_robot[m_mid5].target.angle   = t_opp_ang;

        m_own_robot[m_attack].navigate(m_ref_state.designated_position.circleAroundPoint(t_opp_ang, 75), VelocityProfile::sooski(),
                 NavigationFlags::ForceNoObstacles);

        m_own_robot[m_mid5].navigate(m_ref_state.designated_position.circleAroundPoint(t_ang, 75), VelocityProfile::sooski(),
                 NavigationFlags::ForceNoObstacles);

        bool ball_has_slipt = ((m_own_robot[m_attack].state().position + m_own_robot[m_mid5].state().position) / 2)
                                  .distanceTo(m_world_state.ball.position) > 300;

        if (m_world_state.ball.seen_state == Common::SeenState::Seen && ball_has_slipt)
        {
            m_func_state = 3;
            m_func_count = 0;
        }

        if (m_ref_state.designated_position.circleAroundPoint(t_opp_ang, 75)
                    .distanceTo(m_own_robot[m_attack].state().position) < 40 &&
            m_ref_state.designated_position.circleAroundPoint(t_ang, 75).distanceTo(
                m_own_robot[m_mid5].state().position) < 40)
        {
            m_func_count++;
        }
        if (m_func_count >= 30)
        {
            m_func_state = 6;
            m_func_count = 0;
        }
    }
    else if (m_func_state == 6)
    {
        m_own_robot[m_attack].target.angle = t_ang;
        m_own_robot[m_mid5].target.angle   = t_opp_ang;
        Common::Vec2 target1               = m_ref_state.designated_position.circleAroundPoint(t_opp_ang, 550);
#if 0
        if (isOut(target1))
        {
            m_own_robot[m_attack].navigate(Common::Vec2(), VelocityProfile::sooski());
        }
        else
#endif
        {
            m_own_robot[m_attack].navigate(target1, VelocityProfile::sooski(), NavigationFlags::ForceNoObstacles);
        }
        Common::Vec2 target2 = m_ref_state.designated_position.circleAroundPoint(t_ang, 550);

#if 0
        if (isOut(target1))
        {
            m_own_robot[m_mid5].navigate(Common::Vec2(), VelocityProfile::sooski());
        }
        else
#endif
        {
            m_own_robot[m_mid5].navigate(target2, VelocityProfile::sooski(), NavigationFlags::ForceNoObstacles);
        }

        if (m_ref_state.designated_position.circleAroundPoint(t_opp_ang, 550)
                    .distanceTo(m_own_robot[m_attack].state().position) < 40 &&
            m_ref_state.designated_position.circleAroundPoint(t_ang, 550)
                    .distanceTo(m_own_robot[m_mid5].state().position) < 40)
        {
            m_func_count++;
        }
        if (m_func_count >= 30)
        {
            if (m_world_state.ball.position.distanceTo(m_ref_state.designated_position) > 95)
            {
                m_func_state = 3;
                m_func_count = 0;
            }
            else
            {
                m_func_state = 7;
                m_func_count = 0;
            }
        }
    }
    else
    {
        m_own_robot[m_attack].face(m_world_state.ball.position); // TODO test this
        m_own_robot[m_attack].navigate(m_ref_state.designated_position.circleAroundPoint(t_opp_ang, 550), VelocityProfile::sooski(),
                 NavigationFlags::ForceBallObstacle);
        m_own_robot[m_mid5].face(m_world_state.ball.position);
        m_own_robot[m_mid5].navigate(m_ref_state.designated_position.circleAroundPoint(t_ang, 550), VelocityProfile::sooski(),
                 NavigationFlags::ForceBallObstacle);

        bool success = m_ref_state.designated_position.distanceTo(m_world_state.ball.position) < 100.0;

        Common::logDebug("______IN___STATE_DONE_____");
        if (success)
        {
            Common::logInfo("MADE it!!!");
        }
        else
        {
            Common::logWarning("lost it!!!");
        }
    }
    Common::logInfo("______IN___STATE_{}_____", m_func_state);

    Common::logInfo("___DIS___{}", m_ref_state.designated_position.distanceTo(m_world_state.ball.position));
    Common::logInfo("__OUT__{}", isOut(m_world_state.ball.position));

    Common::debug().draw(m_ref_state.designated_position, Common::Color::red(), 20.0f);
}

} // namespace Tyr::Soccer
