#pragma once

namespace Tyr::Vision
{
class Raw
{
public:
    Raw();
    ~Raw();

    bool receive();
    void process();
    bool camsReady() const;

    bool publish() const;
    void store();

    void updateAddress(const Common::NetworkAddress &t_address);

    bool isConnected() const
    {
        return m_client && m_client->isConnected();
    }

private:
    void processRobots();
    void mergeRobots(Common::TeamColor t_color);

    void processBalls();

private:
    std::unique_ptr<Common::UdpClient> m_client;
    std::unique_ptr<Common::NngServer> m_server;

    Common::RawBallState m_last_raw_ball; // The last position of the locked ball

    bool                       m_packet_received[Common::Setting::kCamCount];
    Protos::SSL_DetectionFrame m_d_frame[Common::Setting::kCamCount];

    Common::RawWorldState m_state;

    Common::Storage m_storage;
};
} // namespace Tyr::Vision
