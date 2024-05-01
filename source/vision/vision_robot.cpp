#include "vision.h"

namespace Tyr::Vision
{
void Vision::ProcessRobots()
{
    int robots_num = 0;

    // Blue Robots
    // First we have to extract the robots!
    robots_num = ExtractBlueRobots();

    // Now lets merge them!
    robots_num = MergeRobots(robots_num);

    // The most important part, The Kalman Filter!
    FilterRobots(robots_num, this->our_color);

    // Yellow Robots
    // First we have to extract the robots!
    robots_num = ExtractYellowRobots();

    // Now lets merge them!
    robots_num = MergeRobots(robots_num);

    // The most important part, The Kalman Filter!
    FilterRobots(robots_num, !(this->our_color));

    // We're almost done, only Prediction remains undone!
    predictRobotsForward();

    // Now we send Robots States to the AI!
    SendStates();
}
int Vision::ExtractBlueRobots()
{
    int ans = 0;
    for (int i = 0; i < Common::Setting::kCamCount; i++)
    {
        if (Common::setting().use_camera[i])
        {
            for (int j = 0; j < std::min((int) Common::Setting::kMaxRobots, frame[i].robots_blue_size()); j++)
            {
                robot[ans] = frame[i].robots_blue(j);
                ans++;
            }
        }
    }
    return ans;
}

int Vision::ExtractYellowRobots()
{
    int ans = 0;
    for (int i = 0; i < Common::Setting::kCamCount; i++)
    {
        if (Common::setting().use_camera[i])
        {
            for (int j = 0; j < std::min((int) Common::Setting::kMaxRobots, frame[i].robots_yellow_size()); j++)
            {
                robot[ans] = frame[i].robots_yellow(j);
                ans++;
            }
        }
    }
    return ans;
}

int Vision::MergeRobots(int num)
{
    int robots_num = 0;
    for (int i = 0; i < num; i++)
    {
        const Common::Vec2 robot_i{robot[i].x(), robot[i].y()};

        for (int j = i + 1; j < num; j++)
        {
            const Common::Vec2 robot_j{robot[j].x(), robot[j].y()};

            if (robot_i.distanceTo(robot_j) < MERGE_DISTANCE)
            {
                robot[i].set_x((robot[i].x() + robot[j].x()) / (float) 2.0);
                robot[i].set_y((robot[i].y() + robot[j].y()) / (float) 2.0);
                robot[j] = robot[num - 1];
                num--;

                j--;
            }
        }
        robots_num++;
    }

    return robots_num;
}

void Vision::FilterRobots(int num, bool own)
{
    float filtout[2][2];
    float filtpos[2];

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        bool found = false;
        for (int j = 0; j < num; j++)
        {
            if (robot[j].robot_id() == i)
            {
                found = true;

                filtpos[0] = robot[j].x() / (float) 10.0;
                filtpos[1] = robot[j].y() / (float) 10.0;

                if (robot_not_seen[own][i] > 0)
                {
                    robot_kalman[own][i].initializePos(filtpos);
                    AngleFilter[own][i].reset();
                }

                robot_not_seen[own][i] = 0;

                robot_kalman[own][i].updatePosition(filtpos, filtout);

                AngleFilter[own][i].AddData((robot[j].orientation() - rawAngles[own][i]) * 61.0f);
                rawAngles[own][i]                  = robot[j].orientation();
                robotState[own][i].AngularVelocity = Common::Angle::fromRad(AngleFilter[own][i].GetCurrent());
                robotState[own][i].angle           = Common::Angle::fromRad(robot[j].orientation());

                // Make sure our filtered velocities are reasonable
                if (std::fabs(robotState[own][i].AngularVelocity.deg()) < 20.0f)
                    robotState[own][i].AngularVelocity.setDeg(0.0f);
            }
        }

        if (!found)
        {
            robot_not_seen[own][i]++;
            if (robot_not_seen[own][i] >= MAX_ROBOT_NOT_SEEN + 1)
                robot_not_seen[own][i] = MAX_ROBOT_NOT_SEEN + 1;

            robotState[own][i].AngularVelocity.setDeg(0.0f);
        }

        else
        {
            robotState[own][i].Position.x = filtout[0][0];
            robotState[own][i].Position.y = filtout[1][0];
            robotState[own][i].velocity.x = filtout[0][1];
            robotState[own][i].velocity.y = filtout[1][1];

            // Make sure our filtered velocities are reasonable
            if ((robotState[own][i].velocity.length()) > ROBOT_ERROR_VELOCITY)
            {
                robotState[own][i].velocity.x = 0.0f;
                robotState[own][i].velocity.y = 0.0f;
            }

            if (std::fabs(robotState[own][i].velocity.x) < IGNORE_PREDICTION * 2.0f)
                robotState[own][i].velocity.x = 0.0f;
            if (std::fabs(robotState[own][i].velocity.y) < IGNORE_PREDICTION * 2.0f)
                robotState[own][i].velocity.y = 0.0f;

            robotState[own][i].Position.x *= 10.0f;
            robotState[own][i].Position.y *= 10.0f;
            robotState[own][i].velocity.x *= 10.0f;
            robotState[own][i].velocity.y *= 10.0f;
        }
    }
}

void Vision::predictRobotsForward()
{
    for (int own = 1; own < 2; own++)
    {
        for (int i = 0; i < Common::Setting::kMaxRobots; i++)
        {
            robotState[own][i].Position =
                robotState[own][i].Position + robotState[own][i].velocity / (PREDICT_STEPS * 2.0f);

            // Predict the robot to go forward
            robotState[own][i].angle =
                robotState[own][i].angle + robotState[own][i].AngularVelocity / (PREDICT_STEPS * 4.0f);
        }
    }

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        if (robotState[0][i].seenState != Common::Seen)
        {
            robotState[0][i].Position.x =
                robotState[0][i].Position.x + m_state->lastCMDS[i][(int) m_state->lastCMDS[i][10].x].x / 1.2f;
            robotState[0][i].Position.y =
                robotState[0][i].Position.y + m_state->lastCMDS[i][(int) m_state->lastCMDS[i][10].x].y / 1.2f;
        }
        else
        {
            for (int j = 0; j < 10; j++)
            {
                robotState[0][i].Position.x = robotState[0][i].Position.x + m_state->lastCMDS[i][j].x / 1.4f;
                robotState[0][i].Position.y = robotState[0][i].Position.y + m_state->lastCMDS[i][j].y / 1.4f;
            }
        }
    }
}

void Vision::SendStates()
{
    m_state->ownRobots_num = 0;
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        robotState[0][i].vision_id = i;

        m_state->ownRobots_num++;
        if (robot_not_seen[0][i] == 0)
        {
            robotState[0][i].seenState = Common::Seen;
        }
        else if (robot_not_seen[0][i] < MAX_ROBOT_NOT_SEEN)
        {
            robotState[0][i].seenState = Common::TemprolilyOut;
        }
        else
        {
            robotState[0][i].seenState = Common::CompletelyOut;
            m_state->ownRobots_num--;
        }

        if (robot_not_seen[0][i] < MAX_ROBOT_SUBSTITUTE)
        {
            robotState[0][i].OutForSubsitute = false;
        }
        else
        {
            robotState[0][i].OutForSubsitute = true;
        }

        m_state->OwnRobot[i] = robotState[0][i];
    }

    m_state->oppRobots_num = 0;
    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        robotState[1][i].vision_id = i;

        m_state->oppRobots_num++;
        if (robot_not_seen[1][i] == 0)
        {
            robotState[1][i].seenState = Common::Seen;
        }
        else if (robot_not_seen[1][i] < MAX_ROBOT_NOT_SEEN)
        {
            robotState[1][i].seenState = Common::TemprolilyOut;
        }
        else
        {
            robotState[1][i].seenState = Common::CompletelyOut;
            m_state->oppRobots_num--;
        }

        m_state->OppRobot[i] = robotState[1][i];
    }
}
} // namespace Tyr::Vision