#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::calculateBallTrajectory()
{
    // TODO: replace with epsilon check
    if (m_world_state.ball.velocity.length() == 0.0f)
        return;

    if (ballHist.size() < 5)
    {
        return;
    }

    static Common::MedianFilter<float> visionM;
    if (m_world_state.ball.velocity.x == 0)
        visionM.AddData(100.0f);
    else
        visionM.AddData(std::fabs(m_world_state.ball.velocity.y / m_world_state.ball.velocity.x));

    bool isVertical = visionM.GetCurrent() > 1.0f;

    std::vector<float>          ball_x;
    std::vector<float>          ball_y;
    Common::MedianFilter<float> dMedian(5);

    int i;
    for (i = ballHist.size() - 1; i >= 0; i--)
    {
        if (ball_x.size() > 1)
        {
            float d;
            if (isVertical)
            {
                ballLine.calculate(ball_x.size(), &ball_y[0], &ball_x[0]);
                d = ballLine.getDisToPoint(Common::Vec2(ballHist[i].position.y, ballHist[i].position.x));
            }
            else
            {
                ballLine.calculate(ball_x.size(), &ball_x[0], &ball_y[0]);
                d = ballLine.getDisToPoint(ballHist[i].position);
            }

            dMedian.AddData(d);
            if ((dMedian.GetCurrent() > 50) && (ballHist.size() - i > 5))
            {
                break;
            }
        }
        ball_x.push_back(ballHist[i].position.x);
        ball_y.push_back(ballHist[i].position.y);
    }

    i++;

    if (isVertical)
        ballLine.chapeKon();

    // ball_line.calculate(ball_x.size(), &ball_x[0], &ball_y[0]);
    if (ballLine.isAmoodi())
    {
        Common::debug().draw(Common::LineSegment{Common::Vec2(ballLine.getXIntercept(), -2000),
                                                 Common::Vec2(ballLine.getXIntercept(), 2000)},
                             Common::Color::purple());
    }
    Common::debug().draw(Common::LineSegment{
        Common::Vec2(m_world_state.ball.position.x, ballLine.getValue(m_world_state.ball.position.x)),
        Common::Vec2(ballHist[i].position.x, ballLine.getValue(ballHist[i].position.x))});

    if (!ballLine.isAmoodi())
    {
        Common::Line new_line(1.0, -ballLine.getSlope(), -ballLine.getIntercept());
        Common::Vec2 ballN, lastN;
        ballN = new_line.closestPoint(m_world_state.ball.position);
        lastN = new_line.closestPoint(ballHist[i].position);
        Common::debug().draw(Common::LineSegment{ballN, lastN},
                             isVertical ? Common::Color::yellow() : Common::Color::red());
    }

    for (i = std::max((int) ballHist.size() - 60, 0); i < ballHist.size(); i += 10)
    {
        // Common::debug().draw({ballHist[i].position,10+10.0*(float(i)/float(ballHist.size()))},Red);
    }

    // Common::debug().draw(ballHist.front().position,ballHist.back().position,Blue);
}
} // namespace Tyr::Soccer
