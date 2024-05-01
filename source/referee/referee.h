#pragma once

namespace Tyr::Referee
{
class Referee
{
private:
    std::unique_ptr<Common::UdpClient> m_udp;

    Common::RefereeState *RefState;
    Common::BallState    *ballData;

    Common::Vec2 LastPlacedBall;
    int          move_hys; // For isKicked

    Common::Timer timer;

    int command_CNT;

    Protos::SSL_Referee         pSSLRef;
    Protos::SSL_Referee_Stage   playTimeStage;
    Protos::SSL_Referee_Command commandState;

public:
    Referee(Common::WorldState *world_state, Common::RefereeState *referee_state);
    bool connectToRefBox();
    bool isConnected();
    bool recieve();
    bool isKicked(Common::Vec2 ballPos);
    void process();

    int oppGK;
};
} // namespace Tyr::Referee
