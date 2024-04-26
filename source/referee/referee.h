#pragma once

#include <network/udp_client.h>
#include <ssl-refbox/referee.pb.h>
#include <state/referee.h>
#include <state/world.h>
#include <timer/timer.h>

namespace Tyr::Referee
{
class Referee
{
private:
    std::unique_ptr<Common::UdpClient> m_udp;

    Common::RefereeState *RefState;
    Common::BallState    *ballData;

    Common::vec2 LastPlacedBall;
    int          move_hys; // For isKicked

    Common::Timer timer;

    int command_CNT;

    Protos::SSL_Referee         pSSLRef;
    Protos::SSL_Referee_Stage   playTimeStage;
    Protos::SSL_Referee_Command commandState;

public:
    Referee(Common::WorldState *world_state, Common::RefereeState *referee_state);
    bool connectToRefBox(void);
    bool isConnected(void);
    bool recieve(void);
    bool isKicked(Common::vec2 ballPos);
    void process(void);

    int oppGK;
};
} // namespace Tyr::Referee
