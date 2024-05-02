#include "vision.h"

namespace Tyr::Vision
{
void Vision::processBalls()
{
    // First we have to extract the balls!
    extractBalls();

    // Now lets merge them!
    mergeBalls();

    // The most important part, The Kalman Filter!
    filterBalls();

    // We're almost done, only Prediction remains undone!
    predictBall();
}

void Vision::extractBalls()
{
    m_d_ball.clear();

    int count = 0;
    for (int i = 0; i < Common::Setting::kCamCount; i++)
    {
        if (Common::setting().use_camera[i])
        {
            for (int j = 0; j < m_d_frame[i].balls_size(); j++)
            {
                m_d_ball.push_back(m_d_frame[i].balls(j));
            }
        }
    }
}

void Vision::mergeBalls()
{
    int balls_num = 0;

    int num = m_d_ball.size();

    for (int i = 0; i < num; i++)
    {
        const Common::Vec2 ball_i{m_d_ball[i].x(), m_d_ball[i].y()};

        for (int j = i + 1; j < num; j++)
        {
            const Common::Vec2 ball_j{m_d_ball[j].x(), m_d_ball[j].y()};

            if (ball_i.distanceTo(ball_j) < Common::setting().merge_distance)
            {
                const Common::Vec2 ball_merged = (ball_i + ball_j) / 2.0f;

                m_d_ball[i].set_x(ball_merged.x);
                m_d_ball[i].set_y(ball_merged.y);
                if (m_d_ball[i].has_z())
                    m_d_ball[i].set_z((m_d_ball[i].z() + m_d_ball[j].z()) / (float) 2.0);

                m_d_ball[j] = m_d_ball[num - 1];
                num--;

                j--;
            }
        }
        balls_num++;
    }

    m_d_ball.resize(balls_num);
}

void Vision::filterBalls()
{
    int   id  = 100;
    float dis = std::numeric_limits<float>::max();

    const Common::Vec2 raw_ball{m_last_raw_ball.x(), m_last_raw_ball.y()};

    for (int i = 0; i < m_d_ball.size(); i++)
    {
        const Common::Vec2 ball_i{m_d_ball[i].x(), m_d_ball[i].y()};

        const float curr_dis = ball_i.distanceTo(raw_ball);
        if (curr_dis < dis)
        {
            dis = curr_dis;
            id  = i;
        }
    }

    if (dis < Common::setting().max_ball_2_frame_dist)
    {
        float filtout[2][2];
        float filtpos[2] = {m_d_ball[id].x() / (float) 10.0, m_d_ball[id].y() / (float) 10.0};
        m_last_raw_ball.CopyFrom(m_d_ball[id]);

        if (m_ball_not_seen > 0)
        {
            m_ball_kalman.initializePos(filtpos);
        }

        m_ball_kalman.updatePosition(filtpos, filtout);

        m_state->ball.Position.x = filtout[0][0];
        m_state->ball.Position.y = filtout[1][0];
        m_state->ball.velocity.x = filtout[0][1];
        m_state->ball.velocity.y = filtout[1][1];

        m_ball_not_seen           = 0;
        m_state->ball.seenState = Common::Seen;
    }

    else
    {
        m_ball_not_seen++;

        if (m_ball_not_seen > Common::setting().max_ball_frame_not_seen)
        {
            if (m_d_ball.size() > 0)
            {
                float filtout[2][2];
                float filtpos[2] = {m_d_ball[id].x() / (float) 10.0, m_d_ball[id].y() / (float) 10.0};
                m_last_raw_ball.CopyFrom(m_d_ball[id]);
                m_ball_kalman.initializePos(filtpos);

                m_ball_kalman.updatePosition(filtpos, filtout);

                m_state->ball.Position.x = filtout[0][0];
                m_state->ball.Position.y = filtout[1][0];
                m_state->ball.velocity.x = filtout[0][1];
                m_state->ball.velocity.y = filtout[1][1];

                m_ball_not_seen           = 0;
                m_state->ball.seenState = Common::Seen;
            }
            else
            {
                m_state->ball.velocity = Common::Vec2{};

                m_state->ball.Position /= 10.0f;

                m_last_raw_ball.set_x(0.0f);
                m_last_raw_ball.set_y(0.0f);

                m_state->ball.seenState = Common::CompletelyOut;
            }
        }
        else
        {
            m_state->ball.velocity /= 10.0f;
            m_state->ball.Position /= 10.0f;

            m_state->ball.seenState = Common::TemprolilyOut;
        }
    }
}

void Vision::predictBall()
{
    m_state->ball.Position /= 100.0f;
    m_state->ball.velocity /= 100.0f;

    float k       = 0.25f; // velocity derate every sec(units (m/s)/s)
    float tsample = (float) 1.0f / (float) Common::setting().vision_frame_rate;

    float t;
    if (m_state->ball.seenState == Common::TemprolilyOut)
        t = tsample;
    else
        t = PREDICT_STEPS * tsample;

    float dist       = m_state->ball.velocity.length() * t - k * (t * t) / 2.0f;
    float vball_pred = m_state->ball.velocity.length() - k * t;

    // if speed turns out to be negative..it means that ball has stopped, so calculate that amount of
    // distance traveled
    if (vball_pred < 0)
    {
        vball_pred = 0.0f;
        dist       = m_state->ball.velocity.lengthSquared() * k / 2.0f;
        // i.e the ball has stopped, so take a newer vision data for the prediction
    }

    if (m_state->ball.velocity.length() > 0)
    {
        m_state->ball.velocity = m_state->ball.velocity.normalized() * vball_pred;
        m_state->ball.Position += m_state->ball.velocity.normalized() * dist;
    }

    m_state->ball.velocity *= 1000.0f;
    m_state->ball.Position *= 1000.0f;
}

} // namespace Tyr::Vision
