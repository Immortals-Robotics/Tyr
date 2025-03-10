#include "filtered.h"

namespace Tyr::Vision
{
Filtered::Filtered()
{
    for (int color_idx = 0; color_idx < 2; ++color_idx)
    {
        for (int robot_idx = 0; robot_idx < Common::Config::Common::kMaxRobots; robot_idx++)
        {
            m_robot_not_seen[color_idx][robot_idx] = std::numeric_limits<int>::max() - 1;
        }
    }

    m_raw_client      = std::make_unique<Common::NngClient>(Common::config().network.raw_world_state_url);
    m_cmd_client      = std::make_unique<Common::NngClient>(Common::config().network.commands_url);
    m_server          = std::make_unique<Common::NngServer>(Common::config().network.world_state_url);
    m_ball_ekf        = std::make_unique<Ekf3D>(1. / Common::config().vision.vision_frame_rate, Common::config().vision.camera_delay);
    m_ball_ekf_future = std::make_unique<Ekf3D>(1. / Common::config().vision.vision_frame_rate, Common::config().vision.camera_delay);
    m_kick_detector   = std::make_unique<KickDetector>();
    m_chip_estimator  = std::make_unique<ChipEstimator>();
}

bool Filtered::receiveRaw()
{
    Protos::Immortals::RawWorldState pb_state;
    if (!m_raw_client->receive(&pb_state))
        return false;

    m_raw_state = Common::RawWorldState(pb_state);
    return true;
}

bool Filtered::receiveCmds()
{
    Protos::Immortals::CommandsWrapper pb_wrapper;
    if (!m_cmd_client->receive(&pb_wrapper))
        return false;

    m_cmd_map.clear();
    const Sender::CommandsWrapper cmd_wrapper{Sender::CommandsWrapper(pb_wrapper)};
    for (const auto &cmd : cmd_wrapper.commands)
    {
        CommandHistory& history = m_cmd_map[cmd.vision_id];
        if (history.size() >= kMaxHist)
            history.pop_front();
        history.push_back(cmd);
    }

    return true;
}

void Filtered::process()
{
    processBalls(Common::config().vision.use_new_ball_kalman);
    processRobots();

    m_state.time = Common::TimePoint::now();
}

bool Filtered::publish() const
{
    Protos::Immortals::WorldState pb_state{};
    m_state.fillProto(&pb_state);

    return m_server->send(m_state.time, pb_state);
}

} // namespace Tyr::Vision
