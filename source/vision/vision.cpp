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

    for (int color_idx = 0; color_idx < 2; ++color_idx)
    {
        for (int robot_idx = 0; robot_idx < Common::Setting::kMaxRobots; robot_idx++)
        {
            m_robot_not_seen[color_idx][robot_idx] = std::numeric_limits<int>::max() - 1;
            m_robot_kalman[color_idx][robot_idx]   = FilteredObject{fast_filter_path, slow_filter_path};
        }
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