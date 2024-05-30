#include "raw.h"

namespace Tyr::Vision
{
Raw::Raw()
{
    m_packet_received.fill(false);

    m_client = std::make_unique<Common::UdpClient>(Common::config().network.vision_address);
    if (!m_client->isConnected())
    {
        Common::logCritical("Failed to connect to Vision UDP");
    }

    m_server = std::make_unique<Common::NngServer>(Common::config().network.raw_world_state_url);
}

void Raw::process()
{
    m_state.clear();

    for (int i = 0; i < Common::Config::Vision::kCamCount; i++)
    {
        if (Common::config().vision.use_camera[i])
        {
            m_state.addFrame(m_d_frame[i]);
        }
    }

    processBalls();
    processRobots();

    m_state.time = Common::TimePoint::now();

    m_packet_received.fill(false);
}

bool Raw::publish() const
{
    Protos::Immortals::RawWorldState pb_state{};
    m_state.fillProto(&pb_state);

    return m_server->send(m_state.time, pb_state);
}
} // namespace Tyr::Vision
