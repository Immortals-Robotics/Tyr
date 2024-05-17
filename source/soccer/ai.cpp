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

    m_world_client = std::make_unique<Common::NngClient>(Common::setting().world_state_url);
    m_ref_client   = std::make_unique<Common::NngClient>(Common::setting().referee_state_url);

    m_strategy_client = std::make_unique<Common::UdpClient>(Common::setting().strategy_address);

    m_cmd_server = std::make_unique<Common::NngServer>(Common::setting().commands_url);

    m_dss = std::make_unique<Dss>(&m_world_state, 1000.f);

    m_current_play = &Ai::haltAll;

    m_gk_intercepting = false;

    m_random_param = 0.0f;
    m_target_str   = -1;

    m_is_defending  = false;
    m_opp_restarted = false;

    m_last_referee = Common::RefereeState::STATE_GAME_OFF;

    m_mark_map[&m_dmf]  = -1;
    m_mark_map[&m_mid1] = -1;
    m_mark_map[&m_mid2] = -1;
    m_mark_map[&m_lw]   = -1;
    m_mark_map[&m_rw]   = -1;

    m_stm_to_ai_num[0] = &m_gk;
    m_stm_to_ai_num[1] = &m_def;
    m_stm_to_ai_num[2] = &m_dmf;
    m_stm_to_ai_num[3] = &m_mid2;
    m_stm_to_ai_num[4] = &m_mid1;
    m_stm_to_ai_num[5] = &m_attack;
    m_stm_to_ai_num[6] = &m_rw;
    m_stm_to_ai_num[7] = &m_lw;

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        m_own_robot[i] = Robot(&m_world_state.own_robot[i]);

        m_one_touch_detector[i].rState = &m_own_robot[i];
        m_one_touch_detector[i].ball   = &m_world_state.ball;
        m_one_touch_detector[i].side   = &m_side;

        m_one_touch_type[i]      = OneTouchType::OneTouch;
        m_one_touch_type_used[i] = false;

        m_allaf_pos[i] = Common::Vec2();
    }

    m_chip_head = Common::Angle::fromDeg(200);

    m_circle_reached_behind_ball = false;
    m_predicted_ball             = Common::Vec2();

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
    Protos::Immortals::RefereeState pb_state;
    if (!m_ref_client->receive(&pb_state, nullptr, true))
        return false;

    m_ref_state = Common::RefereeState(pb_state);
    return true;
}

bool Ai::publishCommands() const
{
    const Common::TimePoint time = Common::TimePoint::now();

    Protos::Immortals::CommandsWrapper pb_wrapper;

    pb_wrapper.set_time(time.microseconds());

    for (int i = 0; i < Common::Setting::kMaxRobots; i++)
    {
        m_own_robot[i].currentCommand().fillProto(pb_wrapper.add_command());
    }

    return m_cmd_server->send(time, pb_wrapper);
}
} // namespace Tyr::Soccer
