#include "filtered.h"

namespace Tyr::Vision
{

void Filtered::processBalls(const bool t_new_kalman)
{
    filterBalls(t_new_kalman);
    if (!t_new_kalman)
    {
        predictBall();
    }
}

void Filtered::newKalmanBall(const Common::Vec2 &t_position, const bool &t_seen, const int &t_camera_id,
                             const ChipEstimator::Ball3D &t_ball_3d = ChipEstimator::Ball3D{Common::Vec2(0, 0)})
{
    m_ball_hist.push_back(t_position);
    if (m_ball_hist.size() > 150)
    {
        m_ball_hist.pop_front();
    }

    for (const auto &p : m_ball_hist)
    {
        Common::debug().draw(Common::Circle(p, 21), Common::Color::blue());
    }

    Common::Vec3 position_3d(t_position.x, t_position.y, 0);
    Common::Vec3 velocity_3d = t_ball_3d.velocity;

// TODO: fix 3d estimator
#if 0
    if (t_ball_3d.position.z > 0)
    {
        auto real_pos = ChipEstimator::projectToGround(Common::Vec3(t_position.x, t_position.y, t_ball_3d.position.z),
                                                       ChipEstimator::getCameraPos(t_camera_id));
        position_3d   = Common::Vec3(real_pos.x, real_pos.y, t_ball_3d.position.z);
    }
#else
    (void) t_camera_id;
#endif

    m_ball_ekf->process(position_3d, velocity_3d, t_seen, m_state.own_robot, m_state.opp_robot);

    Common::Vec3 filtered_position, filtered_velocity;
    m_ball_ekf->getSate(&filtered_position, &filtered_velocity);

    //    m_ball_ekf_future->process(position_3d, velocity_3d, t_seen, m_state.own_robot, m_state.opp_robot);
    //
    //    m_ball_ekf_future->getFutureState(10., m_state.own_robot, m_state.opp_robot,
    //                                      900. * Common::field().ball_model_straight.acc_roll);

    Common::logError("acc roll {}", Common::field().ball_model_straight.acc_roll);
    m_state.ball.position = filtered_position.xy();
    m_state.ball.velocity = filtered_velocity.xy();
    Common::debug().draw(Common::Circle(m_state.ball.position, Common::field().ball_radius + t_ball_3d.position.z / 10),
                         Common::Color::red());
}

void Filtered::filterBalls(const bool t_new_kalman)
{
    const auto &raw_balls = m_raw_state.balls;

    // find the closest ball to last known ball
    int   id  = -1;
    float dis = std::numeric_limits<float>::max();
    for (size_t i = 0; i < raw_balls.size(); i++)
    {
        const float curr_dis = raw_balls[i].position.distanceTo(m_last_raw_ball.position);
        if (curr_dis < dis)
        {
            dis = curr_dis;
            id  = i;
        }
    }

    const bool ball_found = id >= 0;
    const bool ball_changed = dis > Common::config().vision.max_ball_2_frame_dist;

    if (!t_new_kalman)
    {
        m_ball_kalman.predict();
    }

    if (ball_found)
    {
        const Common::RawBallState& raw_ball = raw_balls[id];

        if (ball_changed ||
            m_state.ball.seen_state == Common::SeenState::CompletelyOut)
        {
            if (t_new_kalman)
            {
                m_ball_ekf->init(raw_ball.position.xy());
            }
            else
            {
                m_ball_kalman.reset(raw_ball.position.xy());
            }
        }

        if (t_new_kalman)
        {
            ChipEstimator::Ball3D ball_3d = m_chip_estimator->getBall3D(
            raw_ball.position.xy(), raw_ball.frame.camera_id, m_raw_state.time, m_state.own_robot, m_state.opp_robot);

            newKalmanBall(raw_ball.position.xy(), true, raw_ball.frame.camera_id, ball_3d);
        }
        else
        {
            m_ball_kalman.update(raw_ball.position.xy());
        }

        m_ball_not_seen = 0;

        m_last_raw_ball = raw_ball;
    }
    else
    {
        m_ball_not_seen++;
    }

    if (m_ball_not_seen == 0)
    {
        m_state.ball.seen_state = Common::SeenState::Seen;
    }
    else if (m_ball_not_seen < Common::config().vision.max_ball_frame_not_seen)
    {
        m_state.ball.seen_state = Common::SeenState::TemporarilyOut;
    }
    else
    {
        m_state.ball.seen_state = Common::SeenState::CompletelyOut;
    }

    if (!t_new_kalman)
    {
        m_state.ball.position = m_ball_kalman.getPosition();
        m_state.ball.velocity = m_ball_kalman.getVelocity();
    }
}

void Filtered::predictBall()
{
    // TODO: move these to the ball system model

    // can't predict stationary balls
    if (m_state.ball.velocity.length() == 0.0f)
    {
        return;
    }

    const float k  = 700.f; // ball deceleration (mm/s2)
    const Common::Vec2 deceleration = -m_state.ball.velocity.normalized() * k;

    const float time_to_stop = m_state.ball.velocity.length() / k;

    float dt = 1.0f / Common::config().vision.vision_frame_rate;
    const float prediction_time = std::min(time_to_stop, kPredictSteps * dt);

    Common::Vec2 predicted_velocity = m_state.ball.velocity + deceleration * prediction_time;
    const Common::Vec2 displacement = (m_state.ball.velocity + predicted_velocity) * (0.5f * prediction_time);

    m_state.ball.velocity = predicted_velocity;
    m_state.ball.position += displacement;
}

} // namespace Tyr::Vision
