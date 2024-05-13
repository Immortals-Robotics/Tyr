#include "referee.h"

namespace Tyr::Referee
{
bool Referee::connect()
{
    m_ref_client   = std::make_unique<Common::UdpClient>(Common::setting().referee_address);
    m_world_client = std::make_unique<Common::NngClient>(Common::setting().world_state_url);

    m_server = std::make_unique<Common::NngServer>(Common::setting().referee_state_url);

    return isConnected();
}

bool Referee::isConnected()
{
    return m_ref_client != nullptr && m_ref_client->isConnected();
}

void Referee::process()
{
    if (m_ssl_ref.has_designated_position())
    {
        Common::logDebug("HAS POSITION. BALL TARGET POSITION IS: [{}, {}]", m_ssl_ref.designated_position().x(),
                         m_ssl_ref.designated_position().y());

        m_state.place_ball_target =
            Common::Vec2(m_ssl_ref.designated_position().x(), m_ssl_ref.designated_position().y());
    }

    if (m_cmd_counter != m_ssl_ref.command_counter())
    { // Update only when there is a new command
        m_cmd_counter = m_ssl_ref.command_counter();

        m_last_placed_ball = m_world_state.ball.position;

        if (Common::setting().our_color == Common::TeamColor::Blue)
            m_state.opp_gk = m_ssl_ref.yellow().goalie();
        else
            m_state.opp_gk = m_ssl_ref.blue().goalie();

        m_move_hys = 0; // TODO maybe it needs to be commented

        m_timer.start();

        Common::logDebug("command: {}", (int) m_ssl_ref.command());
        Common::logDebug("m_cmd_counter: {}", m_ssl_ref.command_counter());
    }

    transition(m_ssl_ref.command());
}

bool Referee::publish() const
{
    Protos::Immortals::RefereeState pb_state;
    m_state.fillProto(&pb_state);

    return m_server->send(m_state.time, pb_state);
}

bool Referee::isKicked()
{
    int   requiredHys = 5;
    float requiredDis = 50.0f;
    if (m_state.ourRestart())
    {
        requiredHys = 5;
        requiredDis = 150.0f;
    }

    const float ball_move_dis = m_world_state.ball.position.distanceTo(m_last_placed_ball);
    Common::logDebug("ball has moved {}", ball_move_dis);
    if (ball_move_dis > requiredDis)
    {
        m_move_hys++;
    }
    if (m_move_hys >= requiredHys)
    {
        m_move_hys = 0;
        return true;
    }
    return false;
}

void Referee::transition(const Protos::SSL_Referee_Command ref_command)
{
    const bool ball_kicked = isKicked() || m_timer.time() > 5;

    if (ref_command == Protos::SSL_Referee_Command_HALT)
    {
        m_state.state = Common::RefereeState::STATE_HALTED;
    }
    else if (ref_command == Protos::SSL_Referee_Command_STOP)
    {
        m_state.state = Common::RefereeState::STATE_GAME_OFF;
    }
    else if (ref_command == Protos::SSL_Referee_Command_FORCE_START)
    {
        m_state.state = Common::RefereeState::STATE_GAME_ON;
    }
    else if (ref_command == Protos::SSL_Referee_Command_NORMAL_START &&
             (m_state.state & Common::RefereeState::STATE_NOTREADY))
    {
        m_state.state &= ~Common::RefereeState::STATE_NOTREADY;
        m_state.state |= Common::RefereeState::STATE_READY;
    }
    else if (m_state.state & Common::RefereeState::STATE_READY && ball_kicked)
    {
        m_state.state = Common::RefereeState::STATE_GAME_ON;
    }
    else if (m_state.state == Common::RefereeState::STATE_GAME_OFF)
    {
        switch (ref_command)
        {
        case Protos::SSL_Referee_Command_PREPARE_KICKOFF_BLUE:
            m_state.state = Common::RefereeState::STATE_KICKOFF | Common::RefereeState::STATE_NOTREADY;
            break;
        case Protos::SSL_Referee_Command_PREPARE_KICKOFF_YELLOW:
            m_state.state = Common::RefereeState::STATE_KICKOFF | Common::RefereeState::STATE_NOTREADY;
            break;

        case Protos::SSL_Referee_Command_PREPARE_PENALTY_BLUE:
            m_state.state = Common::RefereeState::STATE_PENALTY | Common::RefereeState::STATE_NOTREADY;
            break;
        case Protos::SSL_Referee_Command_PREPARE_PENALTY_YELLOW:
            m_state.state = Common::RefereeState::STATE_PENALTY | Common::RefereeState::STATE_NOTREADY;
            break;

        case Protos::SSL_Referee_Command_DIRECT_FREE_BLUE:
            m_state.state = Common::RefereeState::STATE_DIRECT | Common::RefereeState::STATE_READY;
            break;
        case Protos::SSL_Referee_Command_DIRECT_FREE_YELLOW:
            m_state.state = Common::RefereeState::STATE_DIRECT | Common::RefereeState::STATE_READY;
            break;

        case Protos::SSL_Referee_Command_INDIRECT_FREE_BLUE:
            m_state.state = Common::RefereeState::STATE_INDIRECT | Common::RefereeState::STATE_READY;
            break;
        case Protos::SSL_Referee_Command_INDIRECT_FREE_YELLOW:
            m_state.state = Common::RefereeState::STATE_INDIRECT | Common::RefereeState::STATE_READY;
            break;

        case Protos::SSL_Referee_Command_BALL_PLACEMENT_BLUE:
            m_state.state = Common::RefereeState::STATE_PLACE_BALL | Common::RefereeState::STATE_NOTREADY;
            break;
        case Protos::SSL_Referee_Command_BALL_PLACEMENT_YELLOW:
            m_state.state = Common::RefereeState::STATE_PLACE_BALL | Common::RefereeState::STATE_NOTREADY;
            break;
        }

        switch (ref_command)
        {
        case Protos::SSL_Referee_Command_PREPARE_KICKOFF_BLUE:
        case Protos::SSL_Referee_Command_PREPARE_PENALTY_BLUE:
        case Protos::SSL_Referee_Command_DIRECT_FREE_BLUE:
        case Protos::SSL_Referee_Command_INDIRECT_FREE_BLUE:
        case Protos::SSL_Referee_Command_BALL_PLACEMENT_BLUE:
            m_state.color = Common::TeamColor::Blue;
            break;

        case Protos::SSL_Referee_Command_PREPARE_KICKOFF_YELLOW:
        case Protos::SSL_Referee_Command_PREPARE_PENALTY_YELLOW:
        case Protos::SSL_Referee_Command_DIRECT_FREE_YELLOW:
        case Protos::SSL_Referee_Command_INDIRECT_FREE_YELLOW:
        case Protos::SSL_Referee_Command_BALL_PLACEMENT_YELLOW:
            m_state.color = Common::TeamColor::Yellow;
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
