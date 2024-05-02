#pragma once

namespace Tyr::Referee
{
class Referee
{
private:
    std::unique_ptr<Common::UdpClient> m_udp;

    Common::RefereeState *m_state;
    Common::WorldState   *m_world_state;

    Common::Vec2 m_last_placed_ball;
    int          m_move_hys; // For isKicked

    Common::Timer m_timer;

    int m_cmd_counter = -1;

    Protos::SSL_Referee m_ssl_ref;

    bool isKicked();

    // This is the state machine transition function
    void transition(Protos::SSL_Referee_Command ref_command);

public:
    Referee(Common::WorldState *world_state, Common::RefereeState *referee_state);
    bool connect();
    bool isConnected();
    bool receive();

    void process();
};
} // namespace Tyr::Referee
