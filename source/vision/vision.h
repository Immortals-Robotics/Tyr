#pragma once

////////////////////////////////////////////////////////////////////
//  vision.h
//
//  This file contains all the functions necessary for processing
//  raw vision data
//
////////////////////////////////////////////////////////////////////

#include "kalman/filtered_object.h"

#ifndef INT_MAX
#define INT_MAX 2147483647 /* maximum (signed) int value */
#endif

#ifndef POWED_DIS
#define POWED_DIS(a, b, c, d) (((a - c) * (a - c)) + ((b - d) * (b - d)))
#endif

#define PREDICT_STEPS 7.0f

#define MAX_BALLS 10
#define MAX_BALL_NOT_SEEN 40

#define MAX_ROBOT_NOT_SEEN 200
#define MAX_ROBOT_SUBSITUTE 60

#define MERGE_DISTANCE 5000

#define BALL_BUFFER_FRAMES 30

namespace Loki::Vision
{
class VisionModule
{
public:
    VisionModule(Common::WorldState *State);
    ~VisionModule();

    bool recievePacket(void);
    bool connectToVisionServer(void);
    void recieveAllCameras(void);
    void ProcessVision(void);
    bool isConnected(void);

    void ProcessRobots(Common::WorldState *);
    int  ExtractBlueRobots(void);
    int  ExtractYellowRobots(void);
    int  MergeRobots(int num);
    void FilterRobots(int num, bool own);
    void predictRobotsForward(Common::WorldState *);
    void SendStates(Common::WorldState *);

    void ProcessBalls(Common::WorldState *);
    int  ExtractBalls(void);
    int  MergeBalls(int num);
    void FilterBalls(int num, Common::WorldState *);
    void predictBallForward(Common::WorldState *);
    void calculateBallHeight(void);

    void ProcessParam(Common::WorldState *);

private:
    bool our_color;
    bool our_side;

    std::unique_ptr<Common::UdpClient> m_visionUDP;
    std::unique_ptr<Common::UdpClient> m_GUIUDP;

    Common::WorldState *playState;

    bool   packet_recieved[Common::Setting::kCamCount];
    Common::vec2 ball_pos_buff[BALL_BUFFER_FRAMES];

    //    int ballBufferIndex;
    //    float ballBufferX[BALL_BUFFER_FRAMES];
    //    float ballBufferY[BALL_BUFFER_FRAMES];

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

    //	double t_capture_buff[MAX_BALLS*Setting::kCamCount];
    //    deque<TVec2> ball_dir_buff;
};
} // namespace Loki::Vision
