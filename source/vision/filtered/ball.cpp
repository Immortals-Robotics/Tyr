#include "filtered.h"

namespace Tyr::Vision
{
void Filtered::processBalls()
{
    const auto &raw_balls = m_raw_state.balls;

    // find the closest ball to last known ball
    const Common::Vec2 last_ball_position = m_tracked_ball.state().position();
    int                id                 = -1;
    float              dis                = std::numeric_limits<float>::max();

    for (size_t i = 0; i < raw_balls.size(); i++)
    {
        const float curr_dis = raw_balls[i].position.xy().distanceTo(last_ball_position);
        if (curr_dis < dis)
        {
            dis = curr_dis;
            id  = i;
        }
    }

    m_tracked_ball.predict();

    const bool any_ball_found = id >= 0;

    const bool current_ball_found = any_ball_found && dis < Common::config().vision.max_ball_2_frame_dist;
    const bool should_switch_ball =
        any_ball_found && !current_ball_found && m_state.ball.seen_state == Common::SeenState::CompletelyOut;

    if (current_ball_found || should_switch_ball)
    {
        const Common::RawBallState &raw_ball = raw_balls[id];

        if (should_switch_ball)
        {
            m_tracked_ball.reset(raw_ball.position.xy());
        }
        else if (current_ball_found)
        {
            if (Common::config().vision.use_ball_3d)
            {
                ChipEstimator::Ball3D ball_3d =
                    m_chip_estimator->getBall3D(raw_ball.position.xy(), raw_ball.frame.camera_id, m_raw_state.time,
                                                m_state.own_robot, m_state.opp_robot);
                if (ball_3d.position.z > 0.f)
                {
                    const auto real_pos = ChipEstimator::projectToGround(
                        Common::Vec3(ball_3d.position.x, ball_3d.position.y, ball_3d.position.z),
                        ChipEstimator::getCameraPos(raw_ball.frame.camera_id));
                    ball_3d.position = Common::Vec3(real_pos.x, real_pos.y, ball_3d.position.z);
                }

                Common::debug().draw(
                    Common::Circle(ball_3d.position.xy(), Common::field().ball_radius + ball_3d.position.z / 10),
                    Common::Color::red());

                m_tracked_ball.update(ball_3d.position.xy());
            }
            else
            {
                m_tracked_ball.update(raw_ball.position.xy());
            }
        }
    }

    const Filter::BallState predicted_state = m_tracked_ball.predict(Common::config().vision.predict_time);

    m_state.ball.position = predicted_state.position();
    m_state.ball.velocity = predicted_state.velocity();

    m_state.ball.position = m_tracked_ball.state().position();
    m_state.ball.velocity = m_tracked_ball.state().velocity();

    if (m_tracked_ball.notSeen() == 0)
    {
        m_state.ball.seen_state = Common::SeenState::Seen;
    }
    else if (m_tracked_ball.notSeen() < Common::config().vision.max_ball_frame_not_seen)
    {
        m_state.ball.seen_state = Common::SeenState::TemporarilyOut;
    }
    else
    {
        m_state.ball.seen_state = Common::SeenState::CompletelyOut;
    }
}
} // namespace Tyr::Vision
