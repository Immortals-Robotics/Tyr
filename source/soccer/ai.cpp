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

    m_cmd_server   = std::make_unique<Common::NngServer>(Common::config().network.commands_url);
    m_state_server = std::make_unique<Common::NngServer>(Common::config().network.soccer_state_url);

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

bool Ai::publishState() const
{
    Common::Soccer::State state;

    state.time = Common::TimePoint::now();

    for (unsigned robot_idx = 0; robot_idx < Common::Config::Common::kMaxRobots; robot_idx++)
    {
        state.robots[robot_idx].id = robot_idx;

        state.robots[robot_idx].shoot    = m_own_robot[robot_idx].shoot();
        state.robots[robot_idx].chip     = m_own_robot[robot_idx].chip();
        state.robots[robot_idx].dribbler = m_own_robot[robot_idx].dribbler();

        state.robots[robot_idx].navigated = m_own_robot[robot_idx].navigated();
        state.robots[robot_idx].halted    = m_own_robot[robot_idx].halted();

        state.robots[robot_idx].one_touch_type = static_cast<Common::Soccer::OneTouchType>(m_one_touch_type[robot_idx]);
        // TODO:
        // state.robots[robot_idx].one_touch_arriving = m_one_touch_detector[robot_idx].arriving;
        state.robots[robot_idx].one_touch_type_used = m_one_touch_type_used[robot_idx];
    }

    state.robots[m_gk].role     = Common::Soccer::Role::Gk;
    state.robots[m_def].role    = Common::Soccer::Role::Def;
    state.robots[m_dmf].role    = Common::Soccer::Role::Dmf;
    state.robots[m_mid2].role   = Common::Soccer::Role::Mid2;
    state.robots[m_mid1].role   = Common::Soccer::Role::Mid1;
    state.robots[m_attack].role = Common::Soccer::Role::Attack;
    state.robots[m_rw].role     = Common::Soccer::Role::Rw;
    state.robots[m_lw].role     = Common::Soccer::Role::Lw;

    for (const auto &[own, opp] : m_mark_map)
    {
        state.robots[*own].mark_target = opp;
    }

    state.random_param = m_random_param;
    state.target_str   = m_target_str;

    state.is_defending  = m_is_defending;
    state.opp_restarted = m_opp_restarted;

    state.gk_intercepting = m_gk_intercepting;

    state.func_state = m_func_state;
    state.func_count = m_func_count;

    Protos::Immortals::Soccer::State pb_state;
    state.fillProto(&pb_state);

    return m_state_server->send(state.time, pb_state);
}
} // namespace Tyr::Soccer
