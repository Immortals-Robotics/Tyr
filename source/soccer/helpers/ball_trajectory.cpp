#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::calculateBallTrajectory()
{
    // TODO: replace with epsilon check
    if (m_world_state.ball.velocity.length() == 0.0f)
        return;

    if (m_ball_hist.size() < 5)
    {
        return;
    }

    static Common::MedianFilter<float> visionM;
    if (m_world_state.ball.velocity.x == 0)
        visionM.add(100.0f);
    else
        visionM.add(std::fabs(m_world_state.ball.velocity.y / m_world_state.ball.velocity.x));

    bool isVertical = visionM.current() > 1.0f;

    std::vector<float>             ball_x;
    std::vector<float>             ball_y;
    Common::MedianFilter<float, 5> dMedian;

    int i;
    for (i = m_ball_hist.size() - 1; i >= 0; i--)
    {
        if (ball_x.size() > 1)
        {
            float d;
            if (isVertical)
            {
                m_ball_line.calculate(ball_x.size(), &ball_y[0], &ball_x[0]);
                d = m_ball_line.getDisToPoint(m_ball_hist[i].position.yx());
            }
            else
            {
                m_ball_line.calculate(ball_x.size(), &ball_x[0], &ball_y[0]);
                d = m_ball_line.getDisToPoint(m_ball_hist[i].position);
            }

            dMedian.add(d);
            if ((dMedian.current() > 50) && (m_ball_hist.size() - i > 5))
            {
                break;
            }
        }
        ball_x.push_back(m_ball_hist[i].position.x);
        ball_y.push_back(m_ball_hist[i].position.y);
    }

    i++;

    if (isVertical)
        m_ball_line.chapeKon();

    if (m_ball_line.isAmoodi())
    {
        Common::debug().draw(Common::LineSegment{Common::Vec2(m_ball_line.getXIntercept(), -2000),
                                                 Common::Vec2(m_ball_line.getXIntercept(), 2000)},
                             Common::Color::purple());
    }
    Common::debug().draw(Common::LineSegment{
        Common::Vec2(m_world_state.ball.position.x, m_ball_line.getValue(m_world_state.ball.position.x)),
        Common::Vec2(m_ball_hist[i].position.x, m_ball_line.getValue(m_ball_hist[i].position.x))});

    if (!m_ball_line.isAmoodi())
    {
        Common::Line new_line(1.0, -m_ball_line.getSlope(), -m_ball_line.getIntercept());
        Common::Vec2 ballN, lastN;
        ballN = new_line.closestPoint(m_world_state.ball.position);
        lastN = new_line.closestPoint(m_ball_hist[i].position);
        Common::debug().draw(Common::LineSegment{ballN, lastN},
                             isVertical ? Common::Color::yellow() : Common::Color::red());
    }

    for (i = 0; i < m_ball_hist.size(); i += 1)
    {
        Common::debug().draw(Common::Circle{m_ball_hist[i].position, 5.0f + 30.0f * (float(i) / float(m_ball_hist.size()))},
                             Common::Color::red().transparent());
    }

    Common::debug().draw(Common::LineSegment{m_ball_hist.front().position, m_ball_hist.back().position},
                         Common::Color::blue().transparent());
}
} // namespace Tyr::Soccer
