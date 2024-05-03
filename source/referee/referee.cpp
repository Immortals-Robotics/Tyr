#include "referee.h"

namespace Tyr::Referee
{
bool Referee::connect()
{
    m_udp = std::make_unique<Common::UdpClient>(Common::setting().referee_address);

    return isConnected();
}

bool Referee::isConnected()
{
    return m_udp != nullptr && m_udp->isConnected();
}

void Referee::process()
{
    if (m_ssl_ref.has_designated_position())
    {
        Common::logDebug("HAS POSITION. BALL TARGET POSITION IS: [{}, {}]", m_ssl_ref.designated_position().x(),
                         m_ssl_ref.designated_position().y());

        Common::refereeState().place_ball_target =
            Common::Vec2(m_ssl_ref.designated_position().x(), m_ssl_ref.designated_position().y());
    }
    else
        Common::logDebug("no new packet received");

    if (m_cmd_counter != m_ssl_ref.command_counter())
    { // Update only when there is a new command
        m_cmd_counter = m_ssl_ref.command_counter();

        m_last_placed_ball = Common::worldState().ball.position;

        if (Common::setting().our_color == Common::TeamColor::Blue)
            Common::refereeState().opp_gk = m_ssl_ref.yellow().goalie();
        else
            Common::refereeState().opp_gk = m_ssl_ref.blue().goalie();

        m_move_hys = 0; // TODO maybe it needs to be commented

        m_timer.start();

        Common::logDebug("command: {}", (int) m_ssl_ref.command());
        Common::logDebug("m_cmd_counter: {}", m_ssl_ref.command_counter());
    }

    transition(m_ssl_ref.command());
}

bool Referee::isKicked()
{
    int   requiredHys = 5;
    float requiredDis = 50.0f;
    if (Common::refereeState().ourRestart())
    {
        requiredHys = 5;
        requiredDis = 150.0f;
    }

    //	std::cout<<"the distance: "<<DIS ( Common::worldState().ball.position , m_last_placed_ball )<<std::endl;
    if (Common::worldState().ball.position.distanceTo(m_last_placed_ball) > requiredDis)
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
        Common::refereeState().state = Common::RefereeState::STATE_HALTED;
    }
    else if (ref_command == Protos::SSL_Referee_Command_STOP)
    {
        Common::refereeState().state = Common::RefereeState::STATE_GAME_OFF;
    }
    else if (ref_command == Protos::SSL_Referee_Command_FORCE_START)
    {
        Common::refereeState().state = Common::RefereeState::STATE_GAME_ON;
    }
    else if (ref_command == Protos::SSL_Referee_Command_NORMAL_START &&
             (Common::refereeState().state & Common::RefereeState::STATE_NOTREADY))
    {
        Common::refereeState().state &= ~Common::RefereeState::STATE_NOTREADY;
        Common::refereeState().state |= Common::RefereeState::STATE_READY;
    }
    else if (Common::refereeState().state & Common::RefereeState::STATE_READY && ball_kicked)
    {
        Common::refereeState().state = Common::RefereeState::STATE_GAME_ON;
    }
    else if (Common::refereeState().state == Common::RefereeState::STATE_GAME_OFF)
    {
        switch (ref_command)
        {
        case Protos::SSL_Referee_Command_PREPARE_KICKOFF_BLUE:
            Common::refereeState().state = Common::RefereeState::STATE_KICKOFF | Common::RefereeState::STATE_NOTREADY;
            break;
        case Protos::SSL_Referee_Command_PREPARE_KICKOFF_YELLOW:
            Common::refereeState().state = Common::RefereeState::STATE_KICKOFF | Common::RefereeState::STATE_NOTREADY;
            break;

        case Protos::SSL_Referee_Command_PREPARE_PENALTY_BLUE:
            Common::refereeState().state = Common::RefereeState::STATE_PENALTY | Common::RefereeState::STATE_NOTREADY;
            break;
        case Protos::SSL_Referee_Command_PREPARE_PENALTY_YELLOW:
            Common::refereeState().state = Common::RefereeState::STATE_PENALTY | Common::RefereeState::STATE_NOTREADY;
            break;

        case Protos::SSL_Referee_Command_DIRECT_FREE_BLUE:
            Common::refereeState().state = Common::RefereeState::STATE_DIRECT | Common::RefereeState::STATE_READY;
            break;
        case Protos::SSL_Referee_Command_DIRECT_FREE_YELLOW:
            Common::refereeState().state = Common::RefereeState::STATE_DIRECT | Common::RefereeState::STATE_READY;
            break;

        case Protos::SSL_Referee_Command_INDIRECT_FREE_BLUE:
            Common::refereeState().state = Common::RefereeState::STATE_INDIRECT | Common::RefereeState::STATE_READY;
            break;
        case Protos::SSL_Referee_Command_INDIRECT_FREE_YELLOW:
            Common::refereeState().state = Common::RefereeState::STATE_INDIRECT | Common::RefereeState::STATE_READY;
            break;

        case Protos::SSL_Referee_Command_BALL_PLACEMENT_BLUE:
            Common::refereeState().state =
                Common::RefereeState::STATE_PLACE_BALL | Common::RefereeState::STATE_NOTREADY;
            break;
        case Protos::SSL_Referee_Command_BALL_PLACEMENT_YELLOW:
            Common::refereeState().state =
                Common::RefereeState::STATE_PLACE_BALL | Common::RefereeState::STATE_NOTREADY;
            break;
        }

        switch (ref_command)
        {
        case Protos::SSL_Referee_Command_PREPARE_KICKOFF_BLUE:
        case Protos::SSL_Referee_Command_PREPARE_PENALTY_BLUE:
        case Protos::SSL_Referee_Command_DIRECT_FREE_BLUE:
        case Protos::SSL_Referee_Command_INDIRECT_FREE_BLUE:
        case Protos::SSL_Referee_Command_BALL_PLACEMENT_BLUE:
            Common::refereeState().color = Common::TeamColor::Blue;
            break;

        case Protos::SSL_Referee_Command_PREPARE_KICKOFF_YELLOW:
        case Protos::SSL_Referee_Command_PREPARE_PENALTY_YELLOW:
        case Protos::SSL_Referee_Command_DIRECT_FREE_YELLOW:
        case Protos::SSL_Referee_Command_INDIRECT_FREE_YELLOW:
        case Protos::SSL_Referee_Command_BALL_PLACEMENT_YELLOW:
            Common::refereeState().color = Common::TeamColor::Yellow;
            break;
        }
    }
}

bool Referee::receive()
{
    if (!isConnected())
        return false;

    if (m_udp->receive(&m_ssl_ref))
    {
        return true;
    }

    return false;
}
} // namespace Tyr::Referee
