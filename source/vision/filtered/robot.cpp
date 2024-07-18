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
    const int color_id = static_cast<int>(t_color);

    auto &raw_robots = t_color == Common::TeamColor::Yellow ? m_raw_state.yellow_robots : m_raw_state.blue_robots;

    auto &robots = Common::config().common.our_color == t_color ? m_state.own_robot : m_state.opp_robot;

    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        auto &robot = robots[i];

        Common::Vec2 filt_pos{};
        Common::Vec2 filt_vel{};

        bool found = false;
        for (size_t j = 0; j < raw_robots.size(); j++)
        {
            auto &raw_robot = raw_robots[j];

            if (raw_robot.id == i)
            {
                found = true;

                if (m_robot_not_seen[color_id][i] > 0)
                {
                    m_robot_kalman[color_id][i].initializePos(raw_robot.position);
                    m_angle_filter[color_id][i].reset();
                }

                m_robot_not_seen[color_id][i] = 0;

                m_robot_kalman[color_id][i].updatePosition(raw_robot.position);
                filt_pos = m_robot_kalman[color_id][i].getPosition();
                filt_vel = m_robot_kalman[color_id][i].getVelocity();

                m_angle_filter[color_id][i].add((raw_robot.angle - m_raw_angles[color_id][i]) *
                                                Common::config().vision.vision_frame_rate);
                m_raw_angles[color_id][i] = raw_robot.angle;

                robot.angular_velocity = m_angle_filter[color_id][i].current();

                robot.angle = raw_robot.angle;

                // Make sure our filtered velocities are reasonable
                if (std::fabs(robot.angular_velocity.deg()) < 20.0f)
                    robot.angular_velocity.setDeg(0.0f);
            }
        }

        if (!found)
        {
            m_robot_not_seen[color_id][i]++;
            if (m_robot_not_seen[color_id][i] >= Common::config().vision.max_robot_frame_not_seen + 1)
                m_robot_not_seen[color_id][i] = Common::config().vision.max_robot_frame_not_seen + 1;

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
    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        Common::RobotState &robot = own_robots[i];

        const CommandHistory& history = m_cmd_map[i];

        if (history.empty())
        {
            Common::logWarning("No command history for robot {}", i);
            robot.position += robot.velocity / (kPredictSteps * 2.0f);
        }
        else
        {
            if (robot.seen_state != Common::SeenState::Seen)
            {
                const Sender::Command& last_cmd = history.back();
                robot.position += last_cmd.motion / 8.5f;
            }
            else
            {
                for (const auto& cmd : history)
                {
                    robot.position += cmd.motion / 10.f;
                }
            }
        }
    }

    auto &opp_robots = m_state.opp_robot;
    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        Common::RobotState &robot = opp_robots[i];
        robot.position += robot.velocity / (kPredictSteps * 2.0f);
        robot.angle += robot.angular_velocity / (kPredictSteps * 4.0f);
    }
}

void Filtered::sendStates()
{
    auto     &own_robots   = m_state.own_robot;
    const int our_color_id = static_cast<int>(Common::config().common.our_color);

    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        own_robots[i].color     = Common::config().common.our_color;
        own_robots[i].vision_id = i;

        if (m_robot_not_seen[our_color_id][i] == 0)
        {
            own_robots[i].seen_state = Common::SeenState::Seen;
        }
        else if (m_robot_not_seen[our_color_id][i] < Common::config().vision.max_robot_frame_not_seen)
        {
            own_robots[i].seen_state = Common::SeenState::TemporarilyOut;
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

    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        opp_robots[i].color     = static_cast<Common::TeamColor>(opp_color_id);
        opp_robots[i].vision_id = i;

        if (m_robot_not_seen[opp_color_id][i] == 0)
        {
            opp_robots[i].seen_state = Common::SeenState::Seen;
        }
        else if (m_robot_not_seen[opp_color_id][i] < Common::config().vision.max_robot_frame_not_seen)
        {
            opp_robots[i].seen_state = Common::SeenState::TemporarilyOut;
        }
        else
        {
            opp_robots[i].seen_state = Common::SeenState::CompletelyOut;
        }
    }
}

} // namespace Tyr::Vision
