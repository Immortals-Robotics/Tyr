#include "Referee.h"

namespace Tyr::Referee
{
Referee::Referee(Common::WorldState *world_state, Common::RefereeState *referee_state)
{
    if (!world_state)
    {
        std::cout << "Referee: \"world_state\" is NULL" << std::endl;
    }
    our_color = Common::setting().our_color == Common::TeamColor::Yellow ? COLOR_YELLOW : COLOR_BLUE;

    command_CNT = -1;

    RefState = referee_state;
    ballData = &(world_state->ball);

    RefState->State->init(our_color);
}

bool Referee::connectToRefBox(void)
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
        //		std::cout<<"HAS POSITION!!!!!"<<std::endl;
        //		std::cout<<"BALL TARGET POSITION
        // IS:"<<pSSLRef.designated_position().x()<<'_'<<pSSLRef.designated_position().y()<<std::endl;
        RefState->placeBallTargetPosition =
            Common::vec2(pSSLRef.designated_position().x(), pSSLRef.designated_position().y());
    }
    //	else
    //		std::cout<<"no new packet received"<<std::endl;

    if (command_CNT != pSSLRef.command_counter())
    { // Update only when there is a new command
        command_CNT = pSSLRef.command_counter();

        LastPlacedBall = ballData->Position;

        if (our_color == TEAM_BLUE)
            RefState->oppGK = pSSLRef.yellow().goalie();
        else
            RefState->oppGK = pSSLRef.blue().goalie();

        move_hys = 0; // TODO maybe it needs to be commented

        timer.start();

        //		std::cout << "command: " << pSSLRef.command() << std::endl;
        //		std::cout << "command_CNT: " << pSSLRef.command_counter() << std::endl;
    }

    RefState->State->transition(pSSLRef.command(), isKicked(ballData->Position) || timer.time() > 5);
    //    if ( isKicked(ballData->Position) )
    //    	std::cout << "kicked" << std::endl;
}

bool Referee::isKicked(Common::vec2 ballPos)
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
    if (Common::distance(ballPos, LastPlacedBall) > requiredDis)
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

bool Referee::recieve(void)
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
