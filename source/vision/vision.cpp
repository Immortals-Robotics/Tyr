#include "vision.h"

namespace Tyr::Vision
{
Vision::Vision(Common::WorldState *t_state)
{
    m_state = t_state;

    for (int i = 0; i < Common::Setting::kCamCount; i++)
        m_packet_received[i] = false;

    m_last_raw_ball.set_x(0.0f);
    m_last_raw_ball.set_y(0.0f);

    const std::filesystem::path fast_filter_path = std::filesystem::path{DATA_DIR} / "ball_filter_fast.txt";
    const std::filesystem::path slow_filter_path = std::filesystem::path{DATA_DIR} / "ball_filter_slow.txt";

    m_ball_kalman = FilteredObject{fast_filter_path, slow_filter_path};

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        m_robot_kalman[0][i] = FilteredObject{fast_filter_path, slow_filter_path};
        m_robot_kalman[1][i] = FilteredObject{fast_filter_path, slow_filter_path};

        m_raw_angles[0][i] = 0.0f;
        m_raw_angles[1][i] = 0.0f;
    }

    if (!connect())
    {
        std::cout << "Failed to connect to Vision UDP" << std::endl;
    }
}

void Vision::receive()
{
    if (!isConnected())
    {
        std::cout << "	Hey you! Put the LAN cable back in its socket, or ..." << std::endl;
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
    processBalls();
    processRobots();

    for (int i = 0; i < Common::Setting::kCamCount; i++)
        m_packet_received[i] = false;
}
} // namespace Tyr::Vision