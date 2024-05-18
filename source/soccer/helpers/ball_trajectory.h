#pragma once

namespace Tyr::Soccer
{
class BallTrajectory
{
public:
    void update(const Common::BallState &t_ball);
    
    void reset()
    {
        m_empty = true;
    }

    void calculate() const;

private:
    static constexpr int kMaxBallHist = 240;

    std::array<Common::Vec2, kMaxBallHist> m_ball_hist;

    bool m_empty = true;

    Common::BallState m_ball;
};
} // namespace Tyr::Soccer
