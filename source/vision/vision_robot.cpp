#include "vision.h"

namespace Tyr::Vision
{
void Vision::processRobots()
{
    const Common::TeamColor our_color = Common::setting().our_color;
    const Common::TeamColor opp_color = (Common::TeamColor)(1 - (int) our_color);

    // Blue Robots
    // First we have to extract the robots!
    extractBlueRobots();

    // Now lets merge them!
    mergeRobots();

    // The most important part, The Kalman Filter!
    filterRobots(our_color);

    // Yellow Robots
    // First we have to extract the robots!
    extractYellowRobots();

    // Now lets merge them!
    mergeRobots();

    // The most important part, The Kalman Filter!
    filterRobots(opp_color);

    // We're almost done, only Prediction remains undone!
    predictRobots();

    // Now we send Robots States to the AI!
    sendStates();
}
void Vision::extractBlueRobots()
{
    m_d_robot.clear();

    for (int i = 0; i < Common::Setting::kCamCount; i++)
    {
        if (Common::setting().use_camera[i])
        {
            for (int j = 0; j < std::min((int) Common::Setting::kMaxRobots, m_d_frame[i].robots_blue_size()); j++)
            {
                m_d_robot.push_back(m_d_frame[i].robots_blue(j));
            }
        }
    }
}

void Vision::extractYellowRobots()
{
    m_d_robot.clear();

    for (int i = 0; i < Common::Setting::kCamCount; i++)
    {
        if (Common::setting().use_camera[i])
        {
            for (int j = 0; j < std::min((int) Common::Setting::kMaxRobots, m_d_frame[i].robots_yellow_size()); j++)
            {
                m_d_robot.push_back(m_d_frame[i].robots_yellow(j));
            }
        }
    }
}

void Vision::mergeRobots()
{
    int num = m_d_robot.size();

    int robots_num = 0;
    for (int i = 0; i < num; i++)
    {
        const Common::Vec2 robot_i{m_d_robot[i].x(), m_d_robot[i].y()};

        for (int j = i + 1; j < num; j++)
        {
            const Common::Vec2 robot_j{m_d_robot[j].x(), m_d_robot[j].y()};

            if (robot_i.distanceTo(robot_j) < Common::setting().merge_distance)
            {
                m_d_robot[i].set_x((m_d_robot[i].x() + m_d_robot[j].x()) / (float) 2.0);
                m_d_robot[i].set_y((m_d_robot[i].y() + m_d_robot[j].y()) / (float) 2.0);
                m_d_robot[j] = m_d_robot[num - 1];
                num--;

                j--;
            }
        }
        robots_num++;
    }

    m_d_robot.resize(robots_num);
}

void Vision::filterRobots(Common::TeamColor t_color)
{
    const int color_id = (int) t_color;

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        Common::Vec2 filt_pos{};
        Common::Vec2 filt_vel{};

        bool found = false;
        for (int j = 0; j < m_d_robot.size(); j++)
        {
            if (m_d_robot[j].robot_id() == i)
            {
                found = true;

                Common::Vec2 filtpos{m_d_robot[j].x(), m_d_robot[j].y()};

                if (m_robot_not_seen[color_id][i] > 0)
                {
                    m_robot_kalman[color_id][i].initializePos(filtpos);
                    m_angle_filter[color_id][i].reset();
                }

                m_robot_not_seen[color_id][i] = 0;

                m_robot_kalman[color_id][i].updatePosition(filtpos);
                filt_pos = m_robot_kalman[color_id][i].getPosition();
                filt_vel = m_robot_kalman[color_id][i].getVelocity();

                m_angle_filter[color_id][i].AddData((m_d_robot[j].orientation() - m_raw_angles[color_id][i]) * 61.0f);
                m_raw_angles[color_id][i] = m_d_robot[j].orientation();
                m_robot_state[color_id][i].angular_velocity =
                    Common::Angle::fromRad(m_angle_filter[color_id][i].GetCurrent());
                m_robot_state[color_id][i].angle = Common::Angle::fromRad(m_d_robot[j].orientation());

                // Make sure our filtered velocities are reasonable
                if (std::fabs(m_robot_state[color_id][i].angular_velocity.deg()) < 20.0f)
                    m_robot_state[color_id][i].angular_velocity.setDeg(0.0f);
            }
        }

        if (!found)
        {
            m_robot_not_seen[color_id][i]++;
            if (m_robot_not_seen[color_id][i] >= Common::setting().max_robot_frame_not_seen + 1)
                m_robot_not_seen[color_id][i] = Common::setting().max_robot_frame_not_seen + 1;

            m_robot_state[color_id][i].angular_velocity.setDeg(0.0f);
        }

        else
        {
            m_robot_state[color_id][i].position = filt_pos;
            m_robot_state[color_id][i].velocity = filt_vel;

            // Make sure our filtered velocities are reasonable
            if ((m_robot_state[color_id][i].velocity.length()) > kRobotErrorVelocity)
            {
                m_robot_state[color_id][i].velocity.x = 0.0f;
                m_robot_state[color_id][i].velocity.y = 0.0f;
            }

            if (std::fabs(m_robot_state[color_id][i].velocity.x) < kIgnorePrediction * 2.0f)
                m_robot_state[color_id][i].velocity.x = 0.0f;
            if (std::fabs(m_robot_state[color_id][i].velocity.y) < kIgnorePrediction * 2.0f)
                m_robot_state[color_id][i].velocity.y = 0.0f;
        }
    }
}

void Vision::predictRobots()
{
    for (int color_id = 1; color_id < 2; color_id++)
    {
        for (int i = 0; i < Common::Setting::kMaxRobots; i++)
        {
            m_robot_state[color_id][i].position =
                m_robot_state[color_id][i].position + m_robot_state[color_id][i].velocity / (kPredictSteps * 2.0f);

            // Predict the robot to go forward
            m_robot_state[color_id][i].angle =
                m_robot_state[color_id][i].angle + m_robot_state[color_id][i].angular_velocity / (kPredictSteps * 4.0f);
        }
    }

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (m_robot_state[0][i].seen_state != Common::SeenState::Seen)
        {
            m_robot_state[0][i].position.x =
                m_robot_state[0][i].position.x + Common::worldState().last_cmds[i][(int) Common::worldState().last_cmds[i][10].x].x / 1.2f;
            m_robot_state[0][i].position.y =
                m_robot_state[0][i].position.y + Common::worldState().last_cmds[i][(int) Common::worldState().last_cmds[i][10].x].y / 1.2f;
        }
        else
        {
            for (int j = 0; j < Common::Setting::kMaxRobots; j++)
            {
                m_robot_state[0][i].position.x = m_robot_state[0][i].position.x + Common::worldState().last_cmds[i][j].x / 1.4f;
                m_robot_state[0][i].position.y = m_robot_state[0][i].position.y + Common::worldState().last_cmds[i][j].y / 1.4f;
            }
        }
    }
}

void Vision::sendStates()
{
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        m_robot_state[0][i].vision_id = i;

        if (m_robot_not_seen[0][i] == 0)
        {
            m_robot_state[0][i].seen_state = Common::SeenState::Seen;
        }
        else if (m_robot_not_seen[0][i] < Common::setting().max_robot_frame_not_seen)
        {
            m_robot_state[0][i].seen_state = Common::SeenState::TemprolilyOut;
        }
        else
        {
            m_robot_state[0][i].seen_state = Common::SeenState::CompletelyOut;
        }

        if (m_robot_not_seen[0][i] < kMaxRobotSubstitute)
        {
            m_robot_state[0][i].out_for_substitute = false;
        }
        else
        {
            m_robot_state[0][i].out_for_substitute = true;
        }

        Common::worldState().own_robot[i] = m_robot_state[0][i];
    }

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        m_robot_state[1][i].vision_id = i;

        if (m_robot_not_seen[1][i] == 0)
        {
            m_robot_state[1][i].seen_state = Common::SeenState::Seen;
        }
        else if (m_robot_not_seen[1][i] < Common::setting().max_robot_frame_not_seen)
        {
            m_robot_state[1][i].seen_state = Common::SeenState::TemprolilyOut;
        }
        else
        {
            m_robot_state[1][i].seen_state = Common::SeenState::CompletelyOut;
        }

        Common::worldState().opp_robot[i] = m_robot_state[1][i];
    }
}
} // namespace Tyr::Vision