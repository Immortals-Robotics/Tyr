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

    m_raw_storage.open("raw-state");
    m_filtered_storage.open("filtered-state");
}

Vision::~Vision()
{
    m_raw_storage.close();
    m_filtered_storage.close();
}

bool Vision::receive()
{
    if (!isConnected())
    {
        Common::logCritical("Hey you! Put the LAN cable back in its socket, or ...");
        return false;
    }

    return receivePacket();
}

bool Vision::camsReady() const
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

    Common::worldState().time = Common::TimePoint::now();

    for (int i = 0; i < Common::Setting::kCamCount; i++)
        m_packet_received[i] = false;

    // TODO: move this to a separate thread
    storeStates();
}

void Vision::storeStates()
{
    Protos::Immortals::RawWorldState pb_raw_state{};
    Common::rawWorldState().fillProto(&pb_raw_state);
    m_raw_storage.store(Common::rawWorldState().time.timestamp(), pb_raw_state);

    Protos::Immortals::WorldState pb_state{};
    Common::worldState().fillProto(&pb_state);
    m_filtered_storage.store(Common::worldState().time.timestamp(), pb_state);
}
} // namespace Tyr::Vision