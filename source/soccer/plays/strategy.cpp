#include "../ai.h"

#include "../tactics/gk.h"
#include "../tactics/receive_pass.h"

namespace Tyr::Soccer
{
bool Ai::loadPlayBook(const std::filesystem::path &t_path)
{
    std::ifstream file(t_path, std::ios::in | std::ios::binary);

    if (!file.is_open())
        return false;

    // get length of file:
    file.seekg(0, std::ios::end);
    size_t length = file.tellg();
    Common::logInfo("reading strategy file \"{}\" with size {}", t_path, length);
    file.seekg(0, std::ios::beg);

    // allocate memory:
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(length);

    // read data as a block:
    file.read(buffer.get(), length);
    file.close();

    Protos::Immortals::PlayBook playbook;
    if (!playbook.ParseFromArray(buffer.get(), length))
    {
        Common::logError("Failed to parse playbook file {} with size {}", t_path, length);
        return false;
    }

    const bool result = setPlayBook(playbook);

    if (result)
        Common::logInfo("Playbook file {} loaded with {} strategies", t_path, m_playbook.strategies.size());
    else
        Common::logCritical("Could not open strategy file {}", t_path);

    return result;
}

bool Ai::receivePlayBook()
{
    Protos::Immortals::PlayBook playbook;
    if (!m_strategy_client->receive(&playbook))
        return false;

    const auto receive_endpoint = m_strategy_client->getLastReceiveEndpoint();
    Common::logInfo("Received playbook from {}", receive_endpoint);

    if (!setPlayBook(playbook))
        return false;

    const std::filesystem::path strategy_path = std::filesystem::path{DATA_DIR} / "strategy.ims";

    std::ofstream strategyFile(strategy_path, std::ios::out | std::ios::binary);
    playbook.SerializePartialToOstream(&strategyFile);
    strategyFile.close();

    return true;
}

bool Ai::setPlayBook(const Protos::Immortals::PlayBook &t_playbook)
{
    if (t_playbook.strategy_size() != t_playbook.weight_size())
    {
        Common::logError("Parsed playbook  has {} strategies != {} weights", t_playbook.strategy_size(),
                         t_playbook.weight_size());
        return false;
    }

    m_playbook = t_playbook;

    for (int strategy_idx = 0; strategy_idx < m_playbook.strategies.size(); ++strategy_idx)
    {
        Common::logInfo("STRATEGY: {}, weight: {}", m_playbook.strategies[strategy_idx].name,
                        m_playbook.weights[strategy_idx]);
    }

    return true;
}

int        step[Common::Config::Common::kMaxRobots]    = {};
float      lastAdv[Common::Config::Common::kMaxRobots] = {};
static int curr_str_id                                 = -1;

bool receivers_reached = false;

void Ai::strategy()
{
    if (m_timer.time().seconds() < 0.5)
    {
        curr_str_id       = m_target_str;
        receivers_reached = false;
    }

    if (curr_str_id == -1)
    {
        haltAll();
        return;
    }

    m_is_defending = false;

    m_assignments.clear();
    createGkAssignment();
    createDefAssignments();
    createMidAssignments();
    createAttackAssignment();

    assignRoles();

    GkTactic{}.execute(m_own_robot[m_gk]);
    defHi(m_own_robot[m_def1], m_own_robot[m_def2], nullptr);

    const Strategy &strategy = m_playbook.strategies[curr_str_id];
    Common::logInfo("STRATEGY: {}", strategy.name);

    const Common::Vec2 sign_modifier{static_cast<float>(m_side), Common::sign(-m_world_state.ball.position.y)};

    Common::logDebug("m_timer: {}", m_timer.time());
    if (m_timer.time().seconds() < 0.5)
    {
        for (int i = 0; i < strategy.roles.size(); i++)
        {
            // FOR NOW: advance to the last step
            step[i]    = std::max(0, static_cast<int>(strategy.roles[i].path.size()) - 2);
            lastAdv[i] = m_timer.time().seconds();
            Common::logDebug("zeroed: {}", i);
        }
        Common::Angle passAngle = Common::Angle::fromDeg(90.0f - m_side * 90.0f);
        circleBall(m_own_robot[m_attack], passAngle, 0, 0);
        return;
    }
    else
    {
        for (int i = 0; i < strategy.roles.size(); i++)
        {
            if (strategy.roles[i].path.size() == 0)
                continue;

            if (step[i] >= strategy.roles[i].path.size() - 1)
            {
                step[i]    = strategy.roles[i].path.size() - 1;
                lastAdv[i] = m_timer.time().seconds();
                Common::logDebug("zeroed: {}", i);
                continue;
            }

            if ((strategy.roles[i].path[step[i]].type == Waypoint::Type::Time) || (*m_strategy_ids[i] == m_attack))
            {
                if (m_timer.time().seconds() - lastAdv[i] > strategy.roles[i].path[step[i]].time * 0.1f)
                {
                    step[i]    = std::min(static_cast<int>(strategy.roles[i].path.size()) - 1, step[i] + 1);
                    lastAdv[i] = m_timer.time().seconds();
                    Common::logDebug("stepped: {}    {}", i, step[i]);
                }
            }
            else
            {
                if ((strategy.roles[i].path[step[i]].position * sign_modifier)
                        .distanceTo(m_own_robot[*m_strategy_ids[i]].state().position) <
                    strategy.roles[i].path[step[i]].tolerance)
                {
                    step[i]    = std::min(static_cast<int>(strategy.roles[i].path.size()) - 1, step[i] + 1);
                    lastAdv[i] = m_timer.time().seconds();
                    Common::logDebug("stepped: {}    {}", i, step[i]);
                }
            }
        }
    }

    bool new_receivers_reached = true;

    for (int i = 0; i < strategy.roles.size(); i++)
    {
        if (strategy.roles[i].path.size() == 0)
            continue;

        if (*m_strategy_ids[i] == m_attack)
        {
            int shoot = 0;
            int chip  = 0;

            if (strategy.roles[i].path[step[i]].type == Waypoint::Type::Position)
            {
                shoot = strategy.roles[i].path[step[i]].tolerance * 100.0f;
                Common::logDebug("ATTACK: shoot: {}", shoot);
            }
            else
            {
                chip = strategy.roles[i].path[step[i]].tolerance;
                Common::logDebug("ATTACK: chip:{}", chip);
            }

            if (step[i] == strategy.roles[i].path.size() - 1 && receivers_reached && m_timer.time().seconds() > 3)
            {
                Common::Angle passAngle =
                    (strategy.roles[i].path[step[i]].position * sign_modifier).angleWith(m_world_state.ball.position);
                circleBall(m_own_robot[*m_strategy_ids[i]], passAngle, shoot, chip);
            }
            else if (step[i] == strategy.roles[i].path.size() - 2)
            {
                Common::Angle passAngle =
                    (strategy.roles[i].path[step[i]].position * sign_modifier).angleWith(m_world_state.ball.position);
                circleBall(m_own_robot[*m_strategy_ids[i]], passAngle, 0, 0, 140.0f);
            }
            else
            {
                Common::Angle passAngle =
                    (strategy.roles[i].path[step[i]].position * sign_modifier).angleWith(m_world_state.ball.position);
                circleBall(m_own_robot[*m_strategy_ids[i]], passAngle, 0, 0);
            }
        }
        else
        {
            const VelocityProfile profile = strategy.roles[i].path[step[i]].velocity_profile;

            if (step[i] != strategy.roles[i].path.size() - 1)
            {
                m_own_robot[*m_strategy_ids[i]].face(Field::oppGoal());
                m_own_robot[*m_strategy_ids[i]].navigate(strategy.roles[i].path[step[i]].position * sign_modifier,
                                                         profile, NavigationFlags::ForceBallObstacle);
            }
            else
            {
                ReceivePassTactic{strategy.roles[i].path[step[i]].position * sign_modifier}.execute(m_own_robot[*m_strategy_ids[i]]);
            }
        }

        switch (strategy.roles[i].afterlife)
        {
        case Role::Afterlife::Gool:
            m_own_robot[*m_strategy_ids[i]].one_touch_type = Common::Soccer::OneTouchType::Gool;
            break;
        case Role::Afterlife::OneTouch:
            m_own_robot[*m_strategy_ids[i]].one_touch_type = Common::Soccer::OneTouchType::OneTouch;
            m_allaf_pos[m_strategy_ids[i]]                 = strategy.roles[i].path.back().position * sign_modifier;

            if (step[i] != strategy.roles[i].path.size() - 1)
                // if (i == m_dmf && remainingDis > 150)
                new_receivers_reached = false;
            break;
        case Role::Afterlife::Shirje:
            m_own_robot[*m_strategy_ids[i]].one_touch_type = Common::Soccer::OneTouchType::Shirje;
            break;
        case Role::Afterlife::Allaf:
            m_own_robot[*m_strategy_ids[i]].one_touch_type = Common::Soccer::OneTouchType::Allaf;
            if (*m_strategy_ids[i] == m_attack)
            {
                m_allaf_pos[m_strategy_ids[i]] = m_world_state.ball.position;
            }
            else
            {
                m_allaf_pos[m_strategy_ids[i]] = strategy.roles[i].path.back().position * sign_modifier;
            }
            break;
        default:
            m_own_robot[*m_strategy_ids[i]].one_touch_type = Common::Soccer::OneTouchType::OneTouch;
            break;
        }
    }

    int zone_idx = 0;
    for (const auto &mid : m_prioritized_mids)
    {
        if (m_own_robot[*mid].navigated())
            continue;

        m_own_robot[*mid].face(m_world_state.ball.position);
        m_own_robot[*mid].navigate(m_sorted_zones[zone_idx]->best_pos, VelocityProfile::mamooli());
        ++zone_idx;
    }

    receivers_reached = new_receivers_reached || m_timer.time().seconds() > 4.5;
}
} // namespace Tyr::Soccer
