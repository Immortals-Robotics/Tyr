#pragma once

#include "ball_estimator.h"

#include "tracked/ball.h"
#include "tracked/robot.h"

namespace Tyr::Vision
{
class Filtered
{
public:
    Filtered();
    ~Filtered() = default;

    bool receiveRaw();
    bool receiveCmds();
    void process();

    bool publish() const;

private:
    void processRobots(Common::TeamColor t_color);

    void processBalls();

private:
    // TODO: move to settings
    static constexpr int   kMaxRobotSubstitute = 60;

    static constexpr int kMaxHistDraw = 200;

    std::unique_ptr<Common::NngClient> m_raw_client;
    std::unique_ptr<Common::NngClient> m_cmd_client;
    std::unique_ptr<Common::NngServer> m_server;

    TrackedBall m_tracked_ball;

    TrackedRobot m_tracked_robot[2][Common::Config::Common::kMaxRobots];

    static constexpr size_t kMaxHist = 100;
    using CommandHistory = std::deque<Sender::Command>;
    std::unordered_map<int, CommandHistory> m_cmd_map;

    Common::RawWorldState m_raw_state;
    Common::WorldState    m_state;

    std::unique_ptr<KickDetector>  m_kick_detector;
    std::unique_ptr<ChipEstimator> m_chip_estimator;
    std::deque<Common::Vec2>       m_ball_hist;
};
} // namespace Tyr::Vision
