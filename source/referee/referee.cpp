#include "referee.h"

namespace Tyr::Referee
{
Referee::Referee(Common::WorldState *world_state, Common::RefereeState *referee_state)
{
    if (!world_state)
    {
        Common::logCritical("Referee: \"world_state\" is NULL");
    }

    command_CNT = -1;

    RefState = referee_state;
    ballData = &(world_state->ball);

    RefState->State->init(Common::setting().our_color);
}

bool Referee::connectToRefBox()
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
    if (pSSLRef.has_designated_position())
    {
        Common::logDebug("HAS POSITION. BALL TARGET POSITION IS: [{}, {}]", pSSLRef.designated_position().x(),
                  pSSLRef.designated_position().y());

        RefState->placeBallTargetPosition =
            Common::Vec2(pSSLRef.designated_position().x(), pSSLRef.designated_position().y());
    }
    else
        Common::logDebug("no new packet received");

    if (command_CNT != pSSLRef.command_counter())
    { // Update only when there is a new command
        command_CNT = pSSLRef.command_counter();

        LastPlacedBall = ballData->Position;

        if (Common::setting().our_color == Common::TeamColor::Blue)
            RefState->oppGK = pSSLRef.yellow().goalie();
        else
            RefState->oppGK = pSSLRef.blue().goalie();

        move_hys = 0; // TODO maybe it needs to be commented

        timer.start();

        Common::logDebug("command: {}", (int) pSSLRef.command());
        Common::logDebug("command_CNT: {}", pSSLRef.command_counter());
    }

    RefState->State->transition(pSSLRef.command(), isKicked(ballData->Position) || timer.time() > 5);
    if (isKicked(ballData->Position))
        Common::logDebug("kicked");
}

bool Referee::isKicked(Common::Vec2 ballPos)
{
    int   requiredHys = 5;
    float requiredDis = 50.0f;
    if (RefState)
    {
        if (RefState->State->ourRestart())
        {
            requiredHys = 5;
            requiredDis = 150.0f;
        }
    }
    //	std::cout<<"the distance: "<<DIS ( ballPos , LastPlacedBall )<<std::endl;
    if (ballPos.distanceTo(LastPlacedBall) > requiredDis)
    {
        move_hys++;
    }
    if (move_hys >= requiredHys)
    {
        move_hys = 0;
        return true;
    }
    return false;
}

bool Referee::recieve()
{
    if (!isConnected())
        return false;

    if (m_udp->receive(&pSSLRef))
    {
        return true;
    }

    return false;
}
} // namespace Tyr::Referee
