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
    Vision(Common::WorldState *t_state);

    void receive();
    void process();
    bool isConnected();

private:
    bool connect();
    bool receivePacket();

    void processRobots();
    int  extractBlueRobots();
    int  extractYellowRobots();
    int  mergeRobots(int num);
    void filterRobots(int num, Common::TeamColor t_color);
    void predictRobots();
    void sendStates();

    void processBalls();
    void extractBalls();
    void mergeBalls();
    void filterBalls();
    void predictBall();

    void processParam();

private:
    // TODO: move to settings
    static constexpr float kPredictSteps       = 7.0f;
    static constexpr int   kMaxRobotSubstitute = 60;

    // Don't add prediction to Ball or Opponents if both velocities are below this threshold
    static constexpr float kIgnorePrediction = 0.045f;

    // If the filtering process yields velocities above these values, reset the filter state
    // All these are in metres/sec
    static constexpr float kRobotErrorVelocity = 450.0f;

    std::unique_ptr<Common::UdpClient> m_udp;

    Common::WorldState *m_state;

    bool m_packet_received[Common::Setting::kCamCount];

    Protos::SSL_DetectionBall m_last_raw_ball; // The last position of the locked ball
    FilteredObject            m_ball_kalman;
    int                       m_ball_not_seen = Common::setting().max_ball_frame_not_seen + 1;

    Common::RobotState m_robot_state[2][Common::Setting::kMaxRobots];
    FilteredObject     m_robot_kalman[2][Common::Setting::kMaxRobots];
    int                m_robot_not_seen[2][Common::Setting::kMaxRobots];

    Common::MedianFilter<float> m_angle_filter[2][Common::Setting::kMaxRobots];
    float                       m_raw_angles[2][Common::Setting::kMaxRobots];

    Protos::SSL_DetectionFrame             m_d_frame[Common::Setting::kCamCount];
    std::vector<Protos::SSL_DetectionBall> m_d_ball;
    Protos::SSL_DetectionRobot             m_d_robot[Common::Setting::kMaxRobots * Common::Setting::kCamCount];
};
} // namespace Tyr::Vision
