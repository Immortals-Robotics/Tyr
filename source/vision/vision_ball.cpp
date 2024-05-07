#include "vision.h"

namespace Tyr::Vision
{
void Vision::processBalls()
{
    // Now lets merge them!
    mergeBalls();

    // The most important part, The Kalman Filter!
    filterBalls();

    // We're almost done, only Prediction remains undone!
    predictBall();
}

void Vision::mergeBalls()
{
    int balls_num = 0;

    auto &balls = Common::rawWorldState().balls;

    int num = balls.size();

    for (int i = 0; i < num; i++)
    {
        for (int j = i + 1; j < num; j++)
        {
            if (balls[i].position.distanceTo(balls[j].position) < Common::setting().merge_distance)
            {
                balls[i].position = (balls[i].position + balls[j].position) / 2.0f;

                balls[j] = balls[num - 1];
                num--;

                j--;
            }
        }
        balls_num++;
    }

    balls.resize(balls_num);
}

void Vision::filterBalls()
{
    auto &balls = Common::rawWorldState().balls;

    int   id  = std::numeric_limits<int>::max();
    float dis = std::numeric_limits<float>::max();

    for (int i = 0; i < balls.size(); i++)
    {
        const float curr_dis = balls[i].position.distanceTo(m_last_raw_ball.position);
        if (curr_dis < dis)
        {
            dis = curr_dis;
            id  = i;
        }
    }

    if (dis < Common::setting().max_ball_2_frame_dist)
    {
        m_last_raw_ball = balls[id];

        if (m_ball_not_seen > 0)
        {
            m_ball_kalman.initializePos(balls[id].position);
        }

        m_ball_kalman.updatePosition(balls[id].position);
        Common::worldState().ball.position = m_ball_kalman.getPosition();
        Common::worldState().ball.velocity = m_ball_kalman.getVelocity();

        m_ball_not_seen                      = 0;
        Common::worldState().ball.seen_state = Common::SeenState::Seen;
    }

    else
    {
        m_ball_not_seen++;

        if (m_ball_not_seen > Common::setting().max_ball_frame_not_seen)
        {
            if (balls.size() > 0)
            {
                m_last_raw_ball = balls[id];
                m_ball_kalman.initializePos(balls[id].position);

                m_ball_kalman.updatePosition(balls[id].position);
                Common::worldState().ball.position = m_ball_kalman.getPosition();
                Common::worldState().ball.velocity = m_ball_kalman.getVelocity();

                m_ball_not_seen                      = 0;
                Common::worldState().ball.seen_state = Common::SeenState::Seen;
            }
            else
            {
                Common::worldState().ball.velocity = Common::Vec2{};

                m_last_raw_ball = Common::RawBallState();

                Common::worldState().ball.seen_state = Common::SeenState::CompletelyOut;
            }
        }
        else
        {
            Common::worldState().ball.seen_state = Common::SeenState::TemprolilyOut;
        }
    }
}

void Vision::predictBall()
{
    Common::worldState().ball.position /= 1000.0f;
    Common::worldState().ball.velocity /= 1000.0f;

    float k       = 0.25f; // velocity derate every sec(units (m/s)/s)
    float tsample = (float) 1.0f / (float) Common::setting().vision_frame_rate;

    float t;
    if (Common::worldState().ball.seen_state == Common::SeenState::TemprolilyOut)
        t = tsample;
    else
        t = kPredictSteps * tsample;

    float dist       = Common::worldState().ball.velocity.length() * t - k * (t * t) / 2.0f;
    float vball_pred = Common::worldState().ball.velocity.length() - k * t;

    // if speed turns out to be negative..it means that ball has stopped, so calculate that amount of
    // distance traveled
    if (vball_pred < 0)
    {
        vball_pred = 0.0f;
        dist       = Common::worldState().ball.velocity.lengthSquared() * k / 2.0f;
        // i.e the ball has stopped, so take a newer vision data for the prediction
    }

    if (Common::worldState().ball.velocity.length() > 0)
    {
        Common::worldState().ball.velocity = Common::worldState().ball.velocity.normalized() * vball_pred;
        Common::worldState().ball.position += Common::worldState().ball.velocity.normalized() * dist;
    }

    Common::worldState().ball.velocity *= 1000.0f;
    Common::worldState().ball.position *= 1000.0f;
}

} // namespace Tyr::Vision
