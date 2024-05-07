#include "vision.h"

namespace Tyr::Vision
{
Vision::Vision()
{
    for (int i = 0; i < Common::Setting::kCamCount; i++)
        m_packet_received[i] = false;

    const std::filesystem::path fast_filter_path = std::filesystem::path{DATA_DIR} / "ball_filter_fast.txt";
    const std::filesystem::path slow_filter_path = std::filesystem::path{DATA_DIR} / "ball_filter_slow.txt";

    m_ball_kalman = FilteredObject{fast_filter_path, slow_filter_path};

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        m_robot_kalman[0][i] = FilteredObject{fast_filter_path, slow_filter_path};
        m_robot_kalman[1][i] = FilteredObject{fast_filter_path, slow_filter_path};
    }

    if (!connect())
    {
        Common::logCritical("Failed to connect to Vision UDP");
    }
}

void Vision::receive()
{
    if (!isConnected())
    {
        Common::logCritical("Hey you! Put the LAN cable back in its socket, or ...");
        return;
    }

    bool cams_ready = false;
    while (!cams_ready)
    {
        cams_ready = true;
        for (int i = 0; i < Common::Setting::kCamCount; i++)
        {
            bool new_cam_ready = m_packet_received[i] || (!Common::setting().use_camera[i]);
            if (!new_cam_ready)
            {
                cams_ready = false;
                break;
            }
        }
        if (cams_ready)
            break;

        receivePacket();
    }
}

void Vision::process()
{
    Protos::SSL_DetectionFrame merged_frame;
    for (int i = 0; i < Common::Setting::kCamCount; i++)
    {
        if (Common::setting().use_camera[i])
        {
            merged_frame.MergeFrom(m_d_frame[i]);
        }
    }

    Common::rawWorldState() = Common::RawWorldState(merged_frame);

    processBalls();
    processRobots();

    for (int i = 0; i < Common::Setting::kCamCount; i++)
        m_packet_received[i] = false;
}
} // namespace Tyr::Vision