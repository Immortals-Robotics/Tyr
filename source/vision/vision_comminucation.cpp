#include "vision.h"

namespace Tyr::Vision
{
bool Vision::connectToVisionServer()
{
    m_udp = std::make_unique<Common::UdpClient>(Common::setting().vision_address);
    return isConnected();
}

bool Vision::recievePacket()
{
    if (!isConnected())
        return false;

    if (m_udp->receive(&packet))
    {
        if (packet.has_detection())
        {
            frame[packet.detection().camera_id()]           = packet.detection();
            packet_recieved[packet.detection().camera_id()] = true;
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