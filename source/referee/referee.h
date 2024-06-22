#pragma once

namespace Tyr::Referee
{
class Referee
{
private:
    std::unique_ptr<Common::UdpClient> m_ref_client;
    std::unique_ptr<Common::NngClient> m_world_client;

    std::unique_ptr<Common::NngServer> m_server;

    Common::WorldState     m_world_state;
    Common::Referee::State m_state;

    Common::Vec2 m_last_ball_pos;
    unsigned     m_move_hys;

    Protos::Ssl::Gc::Referee m_ssl_ref;

    bool isKicked();

    // This is the state machine transition function
    void transition();

public:
    Referee() = default;

    bool connect();
    bool isConnected() const;

    bool receiveRef();
    bool receiveWorld();

    void process();

    bool publish() const;
};
} // namespace Tyr::Referee
