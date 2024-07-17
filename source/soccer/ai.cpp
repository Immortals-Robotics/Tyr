#include "ai.h"

namespace Tyr::Soccer
{
struct RobotProperty
{
    int  serialID;
    bool oldRobot;
    bool hasKick;
    bool hasChip;
    bool hasDribble;
};

Ai::Ai()
{
    Common::logInfo("Running Immortals SSL AI module");
    Common::logInfo("Hope us luck :D ");

    m_world_client = std::make_unique<Common::NngClient>(Common::config().network.world_state_url);
    m_ref_client   = std::make_unique<Common::NngClient>(Common::config().network.referee_state_url);

    m_strategy_client = std::make_unique<Common::UdpClient>(Common::config().network.strategy_address);

    m_cmd_server = std::make_unique<Common::NngServer>(Common::config().network.commands_url);

    m_ball_predictor =
        std::make_unique<Vision::Ekf3D>(1. / Common::config().vision.vision_frame_rate, Common::config().vision.camera_delay);

    m_dss = std::make_unique<Dss>(&m_world_state);

    m_current_play = &Ai::haltAll;

    m_mark_map[&m_dmf]  = -1;
    m_mark_map[&m_mid1] = -1;
    m_mark_map[&m_mid2] = -1;
    m_mark_map[&m_lw]   = -1;
    m_mark_map[&m_rw]   = -1;

    ids = {&m_gk, &m_def, &m_dmf, &m_mid2, &m_mid1, &m_attack, &m_rw, &m_lw};

    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        m_own_robot[i] = Robot(&m_world_state.own_robot[i]);

        m_one_touch_detector[i].rState = &m_own_robot[i];
        m_one_touch_detector[i].ball   = &m_world_state.ball;
        m_one_touch_detector[i].side   = &m_side;

        m_one_touch_type[i]      = OneTouchType::OneTouch;
        m_one_touch_type_used[i] = false;

        m_allaf_pos[i] = Common::Vec2();
    }

    const auto strategy_path = std::filesystem::path(DATA_DIR) / "strategy.ims";
    loadPlayBook(strategy_path);

    m_timer.start();
}

bool Ai::receiveWorld()
{
    Protos::Immortals::WorldState pb_state;
    if (!m_world_client->receive(&pb_state, nullptr, true))
        return false;

    m_world_state = Common::WorldState(pb_state);
    return true;
}

bool Ai::receiveReferee()
{
    Protos::Immortals::Referee::State pb_state;
    if (!m_ref_client->receive(&pb_state, nullptr, true))
        return false;

    m_ref_state = static_cast<Common::Referee::State>(pb_state);
    return true;
}

bool Ai::publishCommands() const
{
    const Common::TimePoint time = Common::TimePoint::now();

    Protos::Immortals::CommandsWrapper pb_wrapper;

    pb_wrapper.set_time(time.microseconds());

    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        m_own_robot[i].currentCommand().fillProto(pb_wrapper.add_command());
    }

    return m_cmd_server->send(time, pb_wrapper);
}
} // namespace Tyr::Soccer
