#include "filtered.h"

namespace Tyr::Vision
{
void Filtered::processRobots()
{
    filterRobots(Common::TeamColor::Yellow);
    filterRobots(Common::TeamColor::Blue);

    predictRobots();

    sendStates();
}

void Filtered::filterRobots(Common::TeamColor t_color)
{
    const int color_id = (int) t_color;

    auto &raw_robots = t_color == Common::TeamColor::Yellow ? m_raw_state.yellow_robots : m_raw_state.blue_robots;

    auto &robots = Common::setting().our_color == t_color ? m_state.own_robot : m_state.opp_robot;

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        auto &robot = robots[i];

        Common::Vec2 filt_pos{};
        Common::Vec2 filt_vel{};

        bool found = false;
        for (int j = 0; j < raw_robots.size(); j++)
        {
            auto &raw_robot = raw_robots[j];

            if (raw_robot.vision_id == i)
            {
                found = true;

                if (m_robot_not_seen[color_id][i] > 0)
                {
                    m_robot_kalman[color_id][i].initializePos(raw_robots[i].position);
                    m_angle_filter[color_id][i].reset();
                }

                m_robot_not_seen[color_id][i] = 0;

                m_robot_kalman[color_id][i].updatePosition(raw_robot.position);
                filt_pos = m_robot_kalman[color_id][i].getPosition();
                filt_vel = m_robot_kalman[color_id][i].getVelocity();

                m_angle_filter[color_id][i].AddData((raw_robot.angle - m_raw_angles[color_id][i]) *
                                                    Common::setting().vision_frame_rate);
                m_raw_angles[color_id][i] = raw_robot.angle;

                robot.angular_velocity = m_angle_filter[color_id][i].GetCurrent();

                robot.angle = raw_robot.angle;

                // Make sure our filtered velocities are reasonable
                if (std::fabs(robot.angular_velocity.deg()) < 20.0f)
                    robot.angular_velocity.setDeg(0.0f);
            }
        }

        if (!found)
        {
            m_robot_not_seen[color_id][i]++;
            if (m_robot_not_seen[color_id][i] >= Common::setting().max_robot_frame_not_seen + 1)
                m_robot_not_seen[color_id][i] = Common::setting().max_robot_frame_not_seen + 1;

            robot.angular_velocity.setDeg(0.0f);
        }

        else
        {
            robot.position = filt_pos;
            robot.velocity = filt_vel;

            // Make sure our filtered velocities are reasonable
            if ((robot.velocity.length()) > kRobotErrorVelocity)
            {
                robot.velocity.x = 0.0f;
                robot.velocity.y = 0.0f;
            }

            if (std::fabs(robot.velocity.x) < kIgnorePrediction * 2.0f)
                robot.velocity.x = 0.0f;
            if (std::fabs(robot.velocity.y) < kIgnorePrediction * 2.0f)
                robot.velocity.y = 0.0f;
        }
    }
}

void Filtered::predictRobots()
{
    auto &own_robots = m_state.own_robot;
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (own_robots[i].seen_state != Common::SeenState::Seen)
        {
            own_robots[i].position.x += m_state.last_cmds[i][(int) m_state.last_cmds[i][10].x].x / 1.2f;
            own_robots[i].position.y += m_state.last_cmds[i][(int) m_state.last_cmds[i][10].x].y / 1.2f;
        }
        else
        {
            for (int j = 0; j < Common::Setting::kMaxRobots; j++)
            {
                own_robots[i].position.x += m_state.last_cmds[i][j].x / 1.4f;
                own_robots[i].position.y += m_state.last_cmds[i][j].y / 1.4f;
            }
        }
    }

    auto &opp_robots = m_state.opp_robot;
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        opp_robots[i].position += opp_robots[i].velocity / (kPredictSteps * 2.0f);
        opp_robots[i].angle += opp_robots[i].angular_velocity / (kPredictSteps * 4.0f);
    }
}

void Filtered::sendStates()
{
    auto     &own_robots   = m_state.own_robot;
    const int our_color_id = (int) Common::setting().our_color;

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        own_robots[i].color     = Common::setting().our_color;
        own_robots[i].vision_id = i;

        if (m_robot_not_seen[our_color_id][i] == 0)
        {
            own_robots[i].seen_state = Common::SeenState::Seen;
        }
        else if (m_robot_not_seen[our_color_id][i] < Common::setting().max_robot_frame_not_seen)
        {
            own_robots[i].seen_state = Common::SeenState::TemprolilyOut;
        }
        else
        {
            own_robots[i].seen_state = Common::SeenState::CompletelyOut;
        }

        if (m_robot_not_seen[our_color_id][i] < kMaxRobotSubstitute)
        {
            own_robots[i].out_for_substitute = false;
        }
        else
        {
            own_robots[i].out_for_substitute = true;
        }
    }

    auto     &opp_robots   = m_state.opp_robot;
    const int opp_color_id = 1 - our_color_id;

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        opp_robots[i].color     = (Common::TeamColor)(opp_color_id);
        opp_robots[i].vision_id = i;

        if (m_robot_not_seen[opp_color_id][i] == 0)
        {
            opp_robots[i].seen_state = Common::SeenState::Seen;
        }
        else if (m_robot_not_seen[opp_color_id][i] < Common::setting().max_robot_frame_not_seen)
        {
            opp_robots[i].seen_state = Common::SeenState::TemprolilyOut;
        }
        else
        {
            opp_robots[i].seen_state = Common::SeenState::CompletelyOut;
        }
    }
}
} // namespace Tyr::Vision