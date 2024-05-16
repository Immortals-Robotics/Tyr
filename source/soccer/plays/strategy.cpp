#include "../ai.h"

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
        Common::logInfo("Playbook file {} loaded with {} strategies", t_path, m_playbook.strategy.size());
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
    Common::logInfo("Received playbook from {} on port {}", receive_endpoint.address().to_string(),
                    receive_endpoint.port());

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

    for (int strategy_idx = 0; strategy_idx < m_playbook.strategy.size(); ++strategy_idx)
    {
        Common::logInfo("STRATEGY: {}, weight: {}", m_playbook.strategy[strategy_idx].name,
                        m_playbook.weight[strategy_idx]);
    }

    return true;
}

int        step[Common::Setting::kMaxRobots]    = {};
float      lastAdv[Common::Setting::kMaxRobots] = {};
static int curr_str_id                          = -1;

bool receivers_reached = false;

void Ai::strategy()
{
    if (timer.time() < 0.5)
    {
        curr_str_id       = target_str;
        receivers_reached = false;
    }

    if (curr_str_id == -1)
    {
        HaltAll();
        return;
    }

    const Strategy &strategy = m_playbook.strategy[curr_str_id];
    Common::logInfo("STRATEGY: {}", strategy.name);

    const Common::Vec2 sign_modifier{(float) side, Common::sign(-m_world_state.ball.position.y)};

    Common::logDebug("timer: {}", timer.time());
    if (timer.time() < 0.5)
    {
        for (int i = 0; i < strategy.role.size(); i++)
        {
            // FOR NOW: advance to the last step
            step[i]    = std::max(0, (int) strategy.role[i].path.size() - 2);
            lastAdv[i] = timer.time();
            Common::logDebug("zeroed: {}", i);
        }
        Common::Angle passAngle = Common::Angle::fromDeg(90.0f - side * 90.0f);
        circle_ball(attack, passAngle, 0, 0, 1.0f);
        return;
    }
    else
    {
        for (int i = 0; i < strategy.role.size(); i++)
        {
            if (strategy.role[i].path.size() == 0)
                continue;

            if (step[i] >= strategy.role[i].path.size() - 1)
            {
                step[i]    = strategy.role[i].path.size() - 1;
                lastAdv[i] = timer.time();
                Common::logDebug("zeroed: {}", i);
                continue;
            }

            if ((strategy.role[i].path[step[i]].type == Waypoint::Type::Time) || (*stm2AInum[i] == attack))
            {
                if (timer.time() - lastAdv[i] > strategy.role[i].path[step[i]].time * 0.1f)
                {
                    step[i]    = std::min((int) strategy.role[i].path.size() - 1, step[i] + 1);
                    lastAdv[i] = timer.time();
                    Common::logDebug("stepped: {}    {}", i, step[i]);
                }
            }
            else
            {
                if ((strategy.role[i].path[step[i]].position * sign_modifier)
                        .distanceTo(OwnRobot[*stm2AInum[i]].state().position) <
                    strategy.role[i].path[step[i]].tolerance)
                {
                    step[i]    = std::min((int) strategy.role[i].path.size() - 1, step[i] + 1);
                    lastAdv[i] = timer.time();
                    Common::logDebug("stepped: {}    {}", i, step[i]);
                }
            }
        }
    }

    bool new_receivers_reached = true;
    DefHi(def, rw, lw, nullptr, true);
    for (int i = 0; i < strategy.role.size(); i++)
    {
        // if ((*stm2AInum[i]==gk)||(*stm2AInum[i]==def)) {
        //	continue;
        // }

        if (strategy.role[i].path.size() == 0)
        {
            if (*stm2AInum[i] == gk)
                GKHi(gk, true);
            else if (*stm2AInum[i] == def && *stm2AInum[i] == lw && *stm2AInum[i] == rw) // No need to halt these guys
                continue;
            else
                Halt(*stm2AInum[i]);
            continue;
        }

        else if (*stm2AInum[i] == attack)
        {
            int shoot = 0;
            int chip  = 0;

            if (strategy.role[i].path[step[i]].type == Waypoint::Type::Position)
            {
                shoot = strategy.role[i].path[step[i]].tolerance;
                Common::logDebug("ATTACK: shoot: {}", shoot);
            }
            else
            {
                chip = strategy.role[i].path[step[i]].tolerance;
                Common::logDebug("ATTACK: chip:{}", chip);
            }

            if (step[i] == strategy.role[i].path.size() - 1 && receivers_reached && timer.time() > 3)
            {
                Common::Angle passAngle =
                    (strategy.role[i].path[step[i]].position * sign_modifier).angleWith(m_world_state.ball.position);
                float tmp_mult = 1; // TODO #11 remove this multiplier and fix that strategy maker
                circle_ball(*stm2AInum[i], passAngle, shoot * tmp_mult, chip, 1.0f);
            }
            else if (step[i] == strategy.role[i].path.size() - 2)
            {
                Common::Angle passAngle =
                    (strategy.role[i].path[step[i]].position * sign_modifier).angleWith(m_world_state.ball.position);
                circle_ball(*stm2AInum[i], passAngle, 0, 0, 1.0f, 140.0f);
            }
            else
            {
                Common::Angle passAngle =
                    (strategy.role[i].path[step[i]].position * sign_modifier).angleWith(m_world_state.ball.position);
                circle_ball(*stm2AInum[i], passAngle, 0, 0, 1.0f);
            }
        }

        else
        {
            ERRTSetObstacles(*stm2AInum[i], true);

            const VelocityProfile::Type profile = strategy.role[i].path[step[i]].velocity_profile;

            if (step[i] != strategy.role[i].path.size() - 1)
            {
                // float dis_to_reach = Common::Vec2::distance(OwnRobot[*stm2AInum[i]].state().position,
                // Common::Vec2(strategy.role[i].path[step[i]].x(),strategy.role[i].path[step[i]].y())); if
                // ((step[i]>=strategy.role[i].path.size()-2) || (dis_to_reach < 500))
                OwnRobot[*stm2AInum[i]].face(Common::Vec2(-side * Common::field().width, 0));
                // else
                //     OwnRobot[*stm2AInum[i]].face(Common::Vec2(strategy.role[i].path[step[i]].x(),strategy.role[i].path[step[i]].y()));
                ERRTNavigate2Point(*stm2AInum[i], strategy.role[i].path[step[i]].position * sign_modifier,
                                   strategy.role[i].path[step[i]].speed, profile);
            }
            else
            {
                receivePass(*stm2AInum[i], strategy.role[i].path[step[i]].position * sign_modifier);
            }
        }

        const float remainingDis = (strategy.role[i].path[step[i]].position * sign_modifier)
                                       .distanceTo(OwnRobot[*stm2AInum[i]].state().position);

        switch (strategy.role[i].afterlife)
        {
        case Role::Afterlife::Gool:
            oneTouchType[*stm2AInum[i]] = gool;
            break;
        case Role::Afterlife::OneTouch:
            oneTouchType[*stm2AInum[i]] = oneTouch;
            if (strategy.role[i].path.size() == 0)
                allafPos[*stm2AInum[i]] = Common::Vec2(0, 0);
            else
                allafPos[*stm2AInum[i]] = strategy.role[i].path.back().position * sign_modifier;

            if (step[i] != strategy.role[i].path.size() - 1)
                // if (i == dmf && remainingDis > 150)
                new_receivers_reached = false;
            break;
        case Role::Afterlife::Shirje:
            oneTouchType[*stm2AInum[i]] = shirje;
            break;
        case Role::Afterlife::Allaf:
            oneTouchType[*stm2AInum[i]] = allaf;
            if (*stm2AInum[i] == attack)
            {
                allafPos[*stm2AInum[i]] = m_world_state.ball.position;
            }
            else
            {
                if (strategy.role[i].path.size() == 0)
                    allafPos[*stm2AInum[i]] = Common::Vec2(0, 0);
                else
                    allafPos[*stm2AInum[i]] = strategy.role[i].path.back().position * sign_modifier;
            }
            break;
        default:
            oneTouchType[*stm2AInum[i]] = oneTouch;
            break;
        }
    }

    receivers_reached = new_receivers_reached || timer.time() > 4.5;
}
} // namespace Tyr::Soccer
