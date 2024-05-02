#include "vision.h"

namespace Tyr::Vision
{
bool Vision::connect()
{
    m_udp = std::make_unique<Common::UdpClient>(Common::setting().vision_address);
    return isConnected();
}

bool Vision::receivePacket()
{
    if (!isConnected())
        return false;

    Protos::SSL_WrapperPacket packet;

    if (m_udp->receive(&packet))
    {
        if (packet.has_detection())
        {
            m_d_frame[packet.detection().camera_id()]           = packet.detection();
            m_packet_received[packet.detection().camera_id()] = true;
        }

        return true;
    }

    return false;
}

bool Vision::isConnected()
{
    return m_udp != nullptr && m_udp->isConnected();
}
} // namespace Tyr::Vision