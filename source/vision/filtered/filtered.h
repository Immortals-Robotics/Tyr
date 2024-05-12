#pragma once

#include "../kalman/filtered_object.h"

namespace Tyr::Vision
{
class Filtered
{
public:
    Filtered();
    ~Filtered();

    bool receive();
    void process();

private:
    void processRobots();
    void filterRobots(Common::TeamColor t_color);
    void predictRobots();
    void sendStates();

    void processBalls();
    void filterBalls();
    void predictBall();

    void storeState();

    bool publishState() const;

private:
    // TODO: move to settings
    static constexpr float kPredictSteps       = 7.0f;
    static constexpr int   kMaxRobotSubstitute = 60;

    // Don't add prediction to Ball or Opponents if both velocities are below this threshold
    static constexpr float kIgnorePrediction = 0.045f;

    // If the filtering process yields velocities above these values, reset the filter state
    // All these are in metres/sec
    static constexpr float kRobotErrorVelocity = 4500.0f;

    std::unique_ptr<Common::NngClient> m_client;
    std::unique_ptr<Common::NngServer> m_server;

    Common::RawBallState m_last_raw_ball; // The last position of the locked ball
    FilteredObject       m_ball_kalman;
    int                  m_ball_not_seen = std::numeric_limits<int>::max() - 1;

    FilteredObject m_robot_kalman[2][Common::Setting::kMaxRobots];
    int            m_robot_not_seen[2][Common::Setting::kMaxRobots];

    Common::MedianFilter<Common::Angle> m_angle_filter[2][Common::Setting::kMaxRobots];
    Common::Angle                       m_raw_angles[2][Common::Setting::kMaxRobots];

    Common::RawWorldState m_raw_state;
    Common::WorldState m_state;

    Common::Storage m_storage;
};
} // namespace Tyr::Vision
