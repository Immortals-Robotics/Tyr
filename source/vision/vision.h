#pragma once

////////////////////////////////////////////////////////////////////
//  vision.h
//
//  This file contains all the functions necessary for processing
//  raw vision data
//
////////////////////////////////////////////////////////////////////

#include "kalman/filtered_object.h"

namespace Tyr::Vision
{
class Vision
{
public:
    Vision();

    bool receive();
    void process();
    bool isConnected();
    bool camsReady() const;

    void updateAddress(const Common::NetworkAddress& t_address);

private:
    bool connect();
    bool receivePacket();

    void processRobots();
    void mergeRobots(Common::TeamColor t_color);
    void filterRobots(Common::TeamColor t_color);
    void predictRobots();
    void sendStates();

    void processBalls();
    void mergeBalls();
    void filterBalls();
    void predictBall();

private:
    // TODO: move to settings
    static constexpr float kPredictSteps       = 7.0f;
    static constexpr int   kMaxRobotSubstitute = 60;

    // Don't add prediction to Ball or Opponents if both velocities are below this threshold
    static constexpr float kIgnorePrediction = 0.045f;

    // If the filtering process yields velocities above these values, reset the filter state
    // All these are in metres/sec
    static constexpr float kRobotErrorVelocity = 4500.0f;

    std::unique_ptr<Common::UdpClient> m_udp;

    bool m_packet_received[Common::Setting::kCamCount];

    Common::RawBallState m_last_raw_ball; // The last position of the locked ball
    FilteredObject       m_ball_kalman;
    int                  m_ball_not_seen = std::numeric_limits<int>::max() - 1;

    FilteredObject m_robot_kalman[2][Common::Setting::kMaxRobots];
    int            m_robot_not_seen[2][Common::Setting::kMaxRobots];

    Common::MedianFilter<Common::Angle> m_angle_filter[2][Common::Setting::kMaxRobots];
    Common::Angle                       m_raw_angles[2][Common::Setting::kMaxRobots];

    Protos::SSL_DetectionFrame m_d_frame[Common::Setting::kCamCount];
};
} // namespace Tyr::Vision
