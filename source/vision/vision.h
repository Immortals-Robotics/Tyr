#pragma once

////////////////////////////////////////////////////////////////////
//  vision.h
//
//  This file contains all the functions necessary for processing
//  raw vision data
//
////////////////////////////////////////////////////////////////////

#include "kalman/filtered_object.h"

#define PREDICT_STEPS 7.0f

#define MAX_BALLS 10
#define MAX_BALL_NOT_SEEN 40

#define MAX_ROBOT_NOT_SEEN 200
#define MAX_ROBOT_SUBSITUTE 60

#define MERGE_DISTANCE 70

namespace Tyr::Vision
{
class Vision
{
public:
    Vision(Common::WorldState *State);
    ~Vision();

    bool recievePacket();
    bool connectToVisionServer();
    void recieveAllCameras();
    void ProcessVision();
    bool isConnected();

private:
    void ProcessRobots();
    int  ExtractBlueRobots();
    int  ExtractYellowRobots();
    int  MergeRobots(int num);
    void FilterRobots(int num, bool own);
    void predictRobotsForward();
    void SendStates();

    void ProcessBalls();
    int  ExtractBalls();
    int  MergeBalls(int num);
    void FilterBalls(int num);
    void predictBallForward();

    void ProcessParam();

private:
    bool our_color;
    bool our_side;

    std::unique_ptr<Common::UdpClient> m_udp;

    Common::WorldState *m_state;

    bool   packet_recieved[Common::Setting::kCamCount];

    Protos::SSL_DetectionBall lastRawBall; // The last position of the locked ball
    FilteredObject            ball_kalman;
    int                       ball_not_seen = MAX_BALL_NOT_SEEN + 1;

    Common::RobotState robotState[2][Common::Setting::kMaxRobots];
    FilteredObject     robot_kalman[2][Common::Setting::kMaxRobots];
    int                robot_not_seen[2][Common::Setting::kMaxRobots];

    Common::MedianFilter<float> AngleFilter[2][Common::Setting::kMaxRobots];
    float                       rawAngles[2][Common::Setting::kMaxRobots];

    Protos::SSL_WrapperPacket  packet;
    Protos::SSL_DetectionFrame frame[Common::Setting::kCamCount];
    Protos::SSL_DetectionBall  d_ball[MAX_BALLS * Common::Setting::kCamCount];
    Protos::SSL_DetectionRobot robot[Common::Setting::kMaxRobots * Common::Setting::kCamCount];
};
} // namespace Tyr::Vision
