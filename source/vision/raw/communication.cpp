#include "raw.h"

namespace Tyr::Vision
{
bool Raw::receive()
{
    Protos::Ssl::Vision::WrapperPacket packet;
    if (m_client->receive(&packet))
    {
        const auto receive_endpoint = m_client->getLastReceiveEndpoint();

        if (packet.has_detection())
        {
            const int camera_id = packet.detection().camera_id();

            if (Common::config().vision.use_camera[camera_id] &&
                packet.detection().frame_number() != m_d_frame[camera_id].frame_number())
            {
                Common::logTrace("Received camera {} frame {} with size {} from {}", camera_id,
                                 packet.detection().frame_number(), packet.ByteSizeLong(), receive_endpoint);

                m_d_frame[camera_id]         = packet.detection();
                m_packet_received[camera_id] = true;
            }
        }

        if (packet.has_geometry())
        {
            Common::logTrace("Received field geometry from {}", receive_endpoint);

            Common::field().update(packet.geometry());
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
    for (int i = 0; i < Common::Config::Vision::kCamCount; i++)
    {
        if (!m_packet_received[i] && Common::config().vision.use_camera[i])
        {
            return false;
        }
    }

    return true;
}
} // namespace Tyr::Vision
