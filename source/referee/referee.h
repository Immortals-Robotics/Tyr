#pragma once

namespace Tyr::Referee
{
class Referee
{
private:
    std::unique_ptr<Common::UdpClient> m_ref_client;
    std::unique_ptr<Common::NngClient> m_world_client;

    std::unique_ptr<Common::NngServer> m_server;

    Common::WorldState   m_world_state;
    Common::RefereeState m_state;

    Common::Vec2 m_last_placed_ball;
    int          m_move_hys; // For isKicked

    Common::Timer m_timer;

    int m_cmd_counter = -1;

    Protos::Ssl::Gc::Referee m_ssl_ref;

    bool isKicked();

    // This is the state machine transition function
    void transition(Protos::Ssl::Gc::Referee_Command ref_command);

public:
    Referee() = default;

    bool connect();
    bool isConnected();

    bool receiveRef();
    bool receiveWorld();

    void process();

    bool publish() const;
};
} // namespace Tyr::Referee
