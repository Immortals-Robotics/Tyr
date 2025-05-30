#include "filtered.h"

namespace Tyr::Vision
{
Filtered::Filtered()
{
    m_raw_client = std::make_unique<Common::NngClient>(Common::config().network.raw_world_state_url);
    m_cmd_client = std::make_unique<Common::NngClient>(Common::config().network.commands_url);
    m_server     = std::make_unique<Common::NngServer>(Common::config().network.world_state_url);
    m_kick_detector  = std::make_unique<KickDetector>();
    m_chip_estimator = std::make_unique<ChipEstimator>();
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

    const Sender::CommandsWrapper cmd_wrapper{Sender::CommandsWrapper(pb_wrapper)};
    for (const auto &cmd : cmd_wrapper.commands)
    {
        CommandHistory &history = m_cmd_map[cmd.vision_id];
        if (history.size() >= kMaxHist)
            history.pop_front();
        history.push_back(cmd);
    }

    return true;
}

void Filtered::process()
{
    processBalls();

    processRobots(Common::TeamColor::Yellow);
    processRobots(Common::TeamColor::Blue);

    m_state.time = Common::TimePoint::now();
}

bool Filtered::publish() const
{
    Protos::Immortals::WorldState pb_state{};
    m_state.fillProto(&pb_state);

    return m_server->send(m_state.time, pb_state);
}

} // namespace Tyr::Vision
