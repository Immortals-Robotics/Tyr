#pragma once

namespace Tyr::Vision
{
class Raw
{
public:
    Raw();
    ~Raw() = default;

    bool receive();
    void process();
    bool publish() const;

    [[nodiscard]] bool camsReady() const;

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

    std::array<bool, Common::Setting::kCamCount> m_packet_received;

    Protos::SSL_DetectionFrame m_d_frame[Common::Setting::kCamCount];

    Common::RawWorldState m_state;
};
} // namespace Tyr::Vision
