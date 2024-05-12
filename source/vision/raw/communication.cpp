#include "raw.h"

namespace Tyr::Vision
{
bool Raw::receive()
{
    Protos::SSL_WrapperPacket packet;
    if (m_client->receive(&packet))
    {
        if (packet.has_detection())
        {
            m_d_frame[packet.detection().camera_id()]         = packet.detection();
            m_packet_received[packet.detection().camera_id()] = true;
        }

        return true;
    }

    return false;
}

void Raw::updateAddress(const Common::NetworkAddress &t_address)
{
    m_client->updateAddress(t_address);
}

bool Raw::camsReady() const
{
    for (int i = 0; i < Common::Setting::kCamCount; i++)
    {
        bool new_cam_ready = m_packet_received[i] || (!Common::setting().use_camera[i]);
        if (!new_cam_ready)
        {
            return false;
        }
    }

    return true;
}
} // namespace Tyr::Vision