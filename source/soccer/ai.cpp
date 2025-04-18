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

    State::m_world = &m_world_state;
    State::m_ref = &m_ref_state;
    State::m_side = &m_side;
    State::m_robots = m_own_robot;
    State::m_timer = &m_timer;

    m_world_client = std::make_unique<Common::NngClient>(Common::config().network.world_state_url);
    m_ref_client   = std::make_unique<Common::NngClient>(Common::config().network.referee_state_url);

    m_strategy_client = std::make_unique<Common::UdpClient>(Common::config().network.strategy_address);

    m_cmd_server   = std::make_unique<Common::NngServer>(Common::config().network.commands_url);
    m_state_server = std::make_unique<Common::NngServer>(Common::config().network.soccer_state_url);

    m_current_play = &Ai::haltAll;

    // create zones
    const unsigned zone_count_x = 2 * (Common::field().width / Zone::kZoneWidth);
    const unsigned zone_count_y = 2 * (Common::field().height / Zone::kZoneHeight);

    for (unsigned i = 0; i < zone_count_x; i++)
    {
        for (unsigned j = 0; j < zone_count_y; j++)
        {
            const Common::Vec2 field_min = Common::Vec2(-Common::field().width, -Common::field().height);
            const Common::Vec2 zone_min  = field_min + Common::Vec2(i * Zone::kZoneWidth, j * Zone::kZoneHeight);

            Zone zone{};

            zone.rect = Common::Rect(zone_min, Zone::kZoneWidth, Zone::kZoneHeight);
            m_zones.push_back(zone);
        }
    }

    m_mark_map[&m_mid1] = -1;
    m_mark_map[&m_mid2] = -1;
    m_mark_map[&m_mid3] = -1;
    m_mark_map[&m_mid4] = -1;
    m_mark_map[&m_mid5] = -1;
    m_mark_map[&m_mid6] = -1;
    m_mark_map[&m_mid7] = -1;

    m_prioritized_mids.push_back(&m_mid5);
    m_prioritized_mids.push_back(&m_mid1);
    m_prioritized_mids.push_back(&m_mid2);
    m_prioritized_mids.push_back(&m_mid3);
    m_prioritized_mids.push_back(&m_mid4);
    m_prioritized_mids.push_back(&m_mid6);
    m_prioritized_mids.push_back(&m_mid7);

    m_ids = {&m_gk, &m_def1, &m_def2, &m_mid1, &m_mid2, &m_mid3, &m_mid4, &m_mid5, &m_mid6, &m_mid7, &m_attack};
    m_strategy_ids = {&m_gk,   &m_def1, &m_mid5, &m_mid1, &m_mid2, &m_attack,
                      &m_def2, &m_mid3, &m_mid4, &m_mid6, &m_mid7};

    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        m_own_robot[i].setState(&m_world_state.own_robot[i]);
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

    state.robots[m_gk].role     = Common::Soccer::Role::Gk;
    state.robots[m_def1].role   = Common::Soccer::Role::Def1;
    state.robots[m_def2].role   = Common::Soccer::Role::Def2;
    state.robots[m_mid1].role   = Common::Soccer::Role::Mid1;
    state.robots[m_mid2].role   = Common::Soccer::Role::Mid2;
    state.robots[m_mid3].role   = Common::Soccer::Role::Mid3;
    state.robots[m_mid4].role   = Common::Soccer::Role::Mid4;
    state.robots[m_mid5].role   = Common::Soccer::Role::Mid5;
    state.robots[m_mid6].role   = Common::Soccer::Role::Mid6;
    state.robots[m_mid7].role   = Common::Soccer::Role::Mid7;
    state.robots[m_attack].role = Common::Soccer::Role::Attack;

    for (unsigned robot_idx = 0; robot_idx < Common::Config::Common::kMaxRobots; robot_idx++)
    {
        state.robots[robot_idx].id = robot_idx;

        if (m_world_state.own_robot[robot_idx].seen_state == Common::SeenState::CompletelyOut)
        {
            state.robots[robot_idx].role = Common::Soccer::Role::Unknown;
            continue;
        }

        state.robots[robot_idx].shoot    = m_own_robot[robot_idx].shoot();
        state.robots[robot_idx].chip     = m_own_robot[robot_idx].chip();
        state.robots[robot_idx].dribbler = m_own_robot[robot_idx].dribbler();

        state.robots[robot_idx].navigated = m_own_robot[robot_idx].navigated();
        state.robots[robot_idx].halted    = m_own_robot[robot_idx].halted();

        state.robots[robot_idx].one_touch_type = m_own_robot[robot_idx].one_touch_type;
        // TODO:
        // state.robots[robot_idx].one_touch_arriving = m_one_touch_detector[robot_idx].arriving;
        state.robots[robot_idx].one_touch_type_used = m_own_robot[robot_idx].one_touch_type_used;
    }

    for (const auto &[own, opp] : m_mark_map)
    {
        state.robots[*own].mark_target = opp;
    }

    state.random_param = m_random_param;
    if (m_target_str != -1)
        state.target_str = m_playbook.strategies.at(m_target_str).name;

    state.is_defending  = m_is_defending;
    state.opp_restarted = m_opp_restarted;

    Protos::Immortals::Soccer::State pb_state;
    state.fillProto(&pb_state);

    return m_state_server->send(state.time, pb_state);
}
} // namespace Tyr::Soccer
