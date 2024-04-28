#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::CalculateBallTrajectory()
{
    // TODO: replace with epsilon check
    if (ball.velocity.length() == 0.0f)
        return;

    if (ballHist.size() < 5)
    {
        return;
    }

    static Common::MedianFilter<float> visionM;
    if (ball.velocity.x == 0)
        visionM.AddData(100.0f);
    else
        visionM.AddData(std::fabs(ball.velocity.y / ball.velocity.x));

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
                d = ballLine.getDisToPoint(Common::Vec2(ballHist[i].Position.y, ballHist[i].Position.x));
            }
            else
            {
                ballLine.calculate(ball_x.size(), &ball_x[0], &ball_y[0]);
                d = ballLine.getDisToPoint(ballHist[i].Position);
            }

            dMedian.AddData(d);
            if ((dMedian.GetCurrent() > 50) && (ballHist.size() - i > 5))
            {
                break;
            }
        }
        ball_x.push_back(ballHist[i].Position.x);
        ball_y.push_back(ballHist[i].Position.y);
    }

    i++;

    if (isVertical)
        ballLine.chapeKon();

    // ball_line.calculate(ball_x.size(), &ball_x[0], &ball_y[0]);
    if (ballLine.isAmoodi())
    {
        Common::debug().drawLineSegment(Common::Vec2(ballLine.getXIntercept(), -2000),
                                        Common::Vec2(ballLine.getXIntercept(), 2000), "", Common::Purple);
    }
    Common::debug().drawLineSegment(Common::Vec2(ball.Position.x, ballLine.getValue(ball.Position.x)),
                                    Common::Vec2(ballHist[i].Position.x, ballLine.getValue(ballHist[i].Position.x)));

    if (!ballLine.isAmoodi())
    {
        Line        new_line(1.0, -ballLine.getSlope(), -ballLine.getIntercept());
        VecPosition ballN, lastN;
        ballN = new_line.getPointOnLineClosestTo(VecPosition(ball.Position.x, ball.Position.y));
        lastN = new_line.getPointOnLineClosestTo(VecPosition(ballHist[i].Position.x, ballHist[i].Position.y));
        Common::debug().drawLineSegment(Common::Vec2(ballN.getX(), ballN.getY()),
                                        Common::Vec2(lastN.getX(), lastN.getY()), "",
                                        isVertical ? Common::Yellow : Common::Red);
    }

    for (i = std::max((int) ballHist.size() - 60, 0); i < ballHist.size(); i += 10)
    {
        // Common::debug().drawCircle(ballHist[i].Position,10+10.0*(float(i)/float(ballHist.size())),Red);
    }

    // Common::debug().drawCircle(Common::Vec2(0, 0),1000,Blue);

    // Common::debug().drawLineSegment(ballHist.front().Position,ballHist.back().Position,Blue);
}
} // namespace Tyr::Soccer
