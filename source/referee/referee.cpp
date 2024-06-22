#include "referee.h"

namespace Tyr::Referee
{
bool Referee::connect()
{
    m_ref_client   = std::make_unique<Common::UdpClient>(Common::config().network.referee_address);
    m_world_client = std::make_unique<Common::NngClient>(Common::config().network.world_state_url);

    m_server = std::make_unique<Common::NngServer>(Common::config().network.referee_state_url);

    return isConnected();
}

bool Referee::isConnected() const
{
    return m_ref_client != nullptr && m_ref_client->isConnected();
}

void Referee::process()
{
    Common::Referee::State old_state = m_state;

    m_state = static_cast<Common::Referee::State>(m_ssl_ref);

    // carry over the old states that are not updated from proto
    m_state.time         = old_state.time;
    m_state.state        = old_state.state;
    m_state.ready        = old_state.ready;
    m_state.color        = old_state.color;
    m_state.last_command = old_state.last_command;

    const Common::Referee::Command command{m_ssl_ref};

    if (m_state.last_command.id != command.id)
    {
        m_state.last_command = command;
        Common::logDebug("received command: {}", command.type);
    }

    transition();

    if (old_state.state != m_state.state)
    {
        m_state.time         = Common::TimePoint::now();
        m_state.last_command = command;

        m_last_ball_pos = m_world_state.ball.position;
        m_move_hys      = 0;

        Common::logDebug("state transition: {} -> {}", old_state, m_state);
    }
}

bool Referee::publish() const
{
    Protos::Immortals::Referee::State pb_state;
    m_state.fillProto(&pb_state);

    return m_server->send(m_state.time, pb_state);
}

bool Referee::isKicked()
{
    const int   required_hys = 5;
    const float required_dis = m_state.our() && m_state.restart() ? 150.0f : 50.0f;

    const float ball_move_dis = m_world_state.ball.position.distanceTo(m_last_ball_pos);
    Common::logDebug("ball has moved {}", ball_move_dis);

    if (ball_move_dis > required_dis)
    {
        m_move_hys++;
    }

    return m_move_hys >= required_hys;
}

void Referee::transition()
{
    // TODO: this 5s should be based on restart type and division A/B
    const Common::Duration timeout = Common::Duration::fromSeconds(5.0f);
    const Common::Duration elapsed = Common::TimePoint::now() - m_state.time;

    const bool ball_kicked = isKicked() || elapsed > timeout;

    if (m_state.last_command.type == Common::Referee::Command::Type::Halt)
    {
        m_state.state = Common::Referee::GameState::Halt;
    }
    else if (m_state.last_command.type == Common::Referee::Command::Type::Stop)
    {
        m_state.state = Common::Referee::GameState::Stop;
    }
    else if (m_state.last_command.type == Common::Referee::Command::Type::ForceStart)
    {
        m_state.state = Common::Referee::GameState::Running;
    }
    else if (m_state.last_command.type == Common::Referee::Command::Type::NormalStart && m_state.restart() &&
             !m_state.ready)
    {
        m_state.ready = true;
    }
    else if (m_state.restart() && m_state.ready && ball_kicked)
    {
        m_state.state = Common::Referee::GameState::Running;
    }
    else if (m_state.state == Common::Referee::GameState::Stop)
    {
        switch (m_state.last_command.type)
        {
        case Common::Referee::Command::Type::PrepareKickoffBlue:
        case Common::Referee::Command::Type::PrepareKickoffYellow:
            m_state.state = Common::Referee::GameState::Kickoff;
            m_state.ready = false;
            break;

        case Common::Referee::Command::Type::PreparePenaltyBlue:
        case Common::Referee::Command::Type::PreparePenaltyYellow:
            m_state.state = Common::Referee::GameState::Penalty;
            m_state.ready = false;
            break;

        case Common::Referee::Command::Type::DirectFreeBlue:
        case Common::Referee::Command::Type::DirectFreeYellow:
            m_state.state = Common::Referee::GameState::FreeKick;
            m_state.ready = true;
            break;

        case Common::Referee::Command::Type::BallPlacementBlue:
        case Common::Referee::Command::Type::BallPlacementYellow:
            m_state.state = Common::Referee::GameState::BallPlacement;
            break;

        default:
            break;
        }

        switch (m_state.last_command.type)
        {
        case Common::Referee::Command::Type::PrepareKickoffBlue:
        case Common::Referee::Command::Type::PreparePenaltyBlue:
        case Common::Referee::Command::Type::DirectFreeBlue:
        case Common::Referee::Command::Type::BallPlacementBlue:
            m_state.color = Common::TeamColor::Blue;
            break;

        case Common::Referee::Command::Type::PrepareKickoffYellow:
        case Common::Referee::Command::Type::PreparePenaltyYellow:
        case Common::Referee::Command::Type::DirectFreeYellow:
        case Common::Referee::Command::Type::BallPlacementYellow:
            m_state.color = Common::TeamColor::Yellow;
            break;

        default:
            break;
        }
    }
}

bool Referee::receiveRef()
{
    return m_ref_client->receive(&m_ssl_ref);
}

bool Referee::receiveWorld()
{
    Protos::Immortals::WorldState pb_state;
    if (!m_world_client->receive(&pb_state))
        return false;

    m_world_state = Common::WorldState(pb_state);
    return true;
}
} // namespace Tyr::Referee
