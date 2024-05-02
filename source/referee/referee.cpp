#include "referee.h"

namespace Tyr::Referee
{
Referee::Referee(Common::WorldState *world_state, Common::RefereeState *referee_state)
{
    if (world_state == nullptr)
    {
        Common::logCritical("world_state is null");
    }

    m_state       = referee_state;
    m_world_state = world_state;

    m_state->state.init(Common::setting().our_color);
}

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

        m_state->place_ball_target =
            Common::Vec2(m_ssl_ref.designated_position().x(), m_ssl_ref.designated_position().y());
    }
    else
        Common::logDebug("no new packet received");

    if (m_cmd_counter != m_ssl_ref.command_counter())
    { // Update only when there is a new command
        m_cmd_counter = m_ssl_ref.command_counter();

        m_last_placed_ball = m_world_state->ball.position;

        if (Common::setting().our_color == Common::TeamColor::Blue)
            m_state->opp_gk = m_ssl_ref.yellow().goalie();
        else
            m_state->opp_gk = m_ssl_ref.blue().goalie();

        m_move_hys = 0; // TODO maybe it needs to be commented

        m_timer.start();

        Common::logDebug("command: {}", (int) m_ssl_ref.command());
        Common::logDebug("m_cmd_counter: {}", m_ssl_ref.command_counter());
    }

    const bool is_kicked = isKicked();
    m_state->state.transition(m_ssl_ref.command(), is_kicked || m_timer.time() > 5);
    if (is_kicked)
        Common::logDebug("kicked");
}

bool Referee::isKicked()
{
    int   requiredHys = 5;
    float requiredDis = 50.0f;
    if (m_state)
    {
        if (m_state->state.ourRestart())
        {
            requiredHys = 5;
            requiredDis = 150.0f;
        }
    }
    //	std::cout<<"the distance: "<<DIS ( m_world_state->ball.position , m_last_placed_ball )<<std::endl;
    if (m_world_state->ball.position.distanceTo(m_last_placed_ball) > requiredDis)
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
