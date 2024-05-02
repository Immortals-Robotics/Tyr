#include "vision.h"

namespace Tyr::Vision
{
void Vision::processRobots()
{
    const Common::TeamColor our_color = Common::setting().our_color;
    const Common::TeamColor opp_color = (Common::TeamColor)(1 - (int) our_color);

    int robots_num = 0;

    // Blue Robots
    // First we have to extract the robots!
    robots_num = extractBlueRobots();

    // Now lets merge them!
    robots_num = mergeRobots(robots_num);

    // The most important part, The Kalman Filter!
    filterRobots(robots_num, our_color);

    // Yellow Robots
    // First we have to extract the robots!
    robots_num = extractYellowRobots();

    // Now lets merge them!
    robots_num = mergeRobots(robots_num);

    // The most important part, The Kalman Filter!
    filterRobots(robots_num, !our_color);

    // We're almost done, only Prediction remains undone!
    predictRobots();

    // Now we send Robots States to the AI!
    sendStates();
}
int Vision::extractBlueRobots()
{
    int ans = 0;
    for (int i = 0; i < Common::Setting::kCamCount; i++)
    {
        if (Common::setting().use_camera[i])
        {
            for (int j = 0; j < std::min((int) Common::Setting::kMaxRobots, m_d_frame[i].robots_blue_size()); j++)
            {
                m_d_robot[ans] = m_d_frame[i].robots_blue(j);
                ans++;
            }
        }
    }
    return ans;
}

int Vision::extractYellowRobots()
{
    int ans = 0;
    for (int i = 0; i < Common::Setting::kCamCount; i++)
    {
        if (Common::setting().use_camera[i])
        {
            for (int j = 0; j < std::min((int) Common::Setting::kMaxRobots, m_d_frame[i].robots_yellow_size()); j++)
            {
                m_d_robot[ans] = m_d_frame[i].robots_yellow(j);
                ans++;
            }
        }
    }
    return ans;
}

int Vision::mergeRobots(int num)
{
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

    return robots_num;
}

void Vision::filterRobots(int num, Common::TeamColor t_color)
{
    const int color_id = (int) t_color;

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        Common::Vec2 filt_pos{};
        Common::Vec2 filt_vel{};

        bool found = false;
        for (int j = 0; j < num; j++)
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
                m_robot_state[color_id][i].AngularVelocity =
                    Common::Angle::fromRad(m_angle_filter[color_id][i].GetCurrent());
                m_robot_state[color_id][i].angle = Common::Angle::fromRad(m_d_robot[j].orientation());

                // Make sure our filtered velocities are reasonable
                if (std::fabs(m_robot_state[color_id][i].AngularVelocity.deg()) < 20.0f)
                    m_robot_state[color_id][i].AngularVelocity.setDeg(0.0f);
            }
        }

        if (!found)
        {
            m_robot_not_seen[color_id][i]++;
            if (m_robot_not_seen[color_id][i] >= Common::setting().max_robot_frame_not_seen + 1)
                m_robot_not_seen[color_id][i] = Common::setting().max_robot_frame_not_seen + 1;

            m_robot_state[color_id][i].AngularVelocity.setDeg(0.0f);
        }

        else
        {
            m_robot_state[color_id][i].Position = filt_pos;
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
            m_robot_state[color_id][i].Position =
                m_robot_state[color_id][i].Position + m_robot_state[color_id][i].velocity / (kPredictSteps * 2.0f);

            // Predict the m_d_robot to go forward
            m_robot_state[color_id][i].angle =
                m_robot_state[color_id][i].angle + m_robot_state[color_id][i].AngularVelocity / (kPredictSteps * 4.0f);
        }
    }

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (m_robot_state[0][i].seenState != Common::Seen)
        {
            m_robot_state[0][i].Position.x =
                m_robot_state[0][i].Position.x + m_state->lastCMDS[i][(int) m_state->lastCMDS[i][10].x].x / 1.2f;
            m_robot_state[0][i].Position.y =
                m_robot_state[0][i].Position.y + m_state->lastCMDS[i][(int) m_state->lastCMDS[i][10].x].y / 1.2f;
        }
        else
        {
            for (int j = 0; j < Common::Setting::kMaxRobots; j++)
            {
                m_robot_state[0][i].Position.x = m_robot_state[0][i].Position.x + m_state->lastCMDS[i][j].x / 1.4f;
                m_robot_state[0][i].Position.y = m_robot_state[0][i].Position.y + m_state->lastCMDS[i][j].y / 1.4f;
            }
        }
    }
}

void Vision::sendStates()
{
    m_state->ownRobots_num = 0;
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        m_robot_state[0][i].vision_id = i;

        m_state->ownRobots_num++;
        if (m_robot_not_seen[0][i] == 0)
        {
            m_robot_state[0][i].seenState = Common::Seen;
        }
        else if (m_robot_not_seen[0][i] < Common::setting().max_robot_frame_not_seen)
        {
            m_robot_state[0][i].seenState = Common::TemprolilyOut;
        }
        else
        {
            m_robot_state[0][i].seenState = Common::CompletelyOut;
            m_state->ownRobots_num--;
        }

        if (m_robot_not_seen[0][i] < kMaxRobotSubstitute)
        {
            m_robot_state[0][i].OutForSubsitute = false;
        }
        else
        {
            m_robot_state[0][i].OutForSubsitute = true;
        }

        m_state->OwnRobot[i] = m_robot_state[0][i];
    }

    m_state->oppRobots_num = 0;
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        m_robot_state[1][i].vision_id = i;

        m_state->oppRobots_num++;
        if (m_robot_not_seen[1][i] == 0)
        {
            m_robot_state[1][i].seenState = Common::Seen;
        }
        else if (m_robot_not_seen[1][i] < Common::setting().max_robot_frame_not_seen)
        {
            m_robot_state[1][i].seenState = Common::TemprolilyOut;
        }
        else
        {
            m_robot_state[1][i].seenState = Common::CompletelyOut;
            m_state->oppRobots_num--;
        }

        m_state->OppRobot[i] = m_robot_state[1][i];
    }
}
} // namespace Tyr::Vision