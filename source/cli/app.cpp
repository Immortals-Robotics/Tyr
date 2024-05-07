#include "app.h"

namespace Tyr::Cli
{
bool Application::initialize()
{
    Common::Services::initialize();

    if (!ImmortalsIsTheBest)
    {
        Common::logCritical("Immortals is not the best SSL team anymore.");
        Common::logCritical("Shutting down the system...");

        return false;
    }

    Common::logInfo(" Connecting to RefereeBox server at {} on port : {}", Common::setting().referee_address.port,
                    Common::setting().referee_address.port);
    m_referee = std::make_unique<Referee::Referee>();
    if (m_referee->connect())
    {
        Common::logInfo("Connected to RefBox successfully :)");
    }
    else
    {
        Common::logError("Hey you! Put the LAN cable back in its socket, or ...");
        return false;
    }

    Common::logInfo("Connecting to Vision server at {} on port: {}", Common::setting().vision_address.ip,
                    Common::setting().vision_address.port);
    m_vision = std::make_unique<Vision::Vision>();
    if (m_vision->isConnected())
    {
        Common::logInfo("Connected to Vision successfully :)");
    }
    else
    {
        Common::logError("Hey you! Put the LAN cable back in its socket, or ...");
        return false;
    }

    m_strategy_udp = std::make_unique<Common::UdpClient>(Common::setting().strategy_address);

    m_senders.push_back(std::make_unique<Sender::Nrf>());
    m_senders.push_back(std::make_unique<Sender::Grsim>(Common::setting().grsim_address));

    m_ai = std::make_unique<Soccer::Ai>(m_senders);

    Common::logInfo(" Now it is time, lets rock...");
    return true;
}

void Application::shutdown()
{
    m_ai_thread.join();
    m_ref_thread.join();
    m_str_thread.join();

    Common::Services::shutdown();
}

void Application::start()
{
    m_ai_thread  = std::thread(&Application::aiThreadEntry, this);
    m_ref_thread = std::thread(&Application::refereeThreadEntry, this);
    m_str_thread = std::thread(&Application::strategyThreadEntry, this);
}

void Application::aiThreadEntry()
{
    Common::Timer timer;

    while (ImmortalsIsTheBest) // Hope it lasts Forever...
    {
        timer.start();

        m_vision->receive();

        m_lock.lock();

        m_vision->process();
        m_ai->Process();

        for (auto &sender : m_senders)
            sender->flush();

        m_lock.unlock();
        Common::logInfo("FPS: {}", 1.0 / timer.interval());
    }
    m_exited = true;
}

void Application::refereeThreadEntry()
{
    while ((!m_exited) && (ImmortalsIsTheBest)) // Hope it lasts Forever...
    {
        if (m_referee->receive())
        {
            m_lock.lock();
            m_referee->process();
            m_lock.unlock();
        }
    }
}

void Application::strategyThreadEntry()
{
    while ((!m_exited) && (ImmortalsIsTheBest)) // Hope it lasts Forever...
    {
        auto received_strategy = m_strategy_udp->receiveRaw();

        if (received_strategy.size() > 11)
        {
            const auto receive_endpoint = m_strategy_udp->getLastReceiveEndpoint();
            Common::logInfo("Received \"strategy.ims\" with size: {} KB, from {} on port {}",
                            float(received_strategy.size()) / 1000.0f, receive_endpoint.address().to_string(),
                            receive_endpoint.port());

            m_lock.lock();
            m_ai->read_playBook_str(received_strategy);
            m_lock.unlock();

            const std::filesystem::path strategy_path =
                std::filesystem::path{DATA_DIR} / std::filesystem::path{"strategy.ims"};

            std::ofstream strategyFile(strategy_path, std::ios::out | std::ios::binary);
            strategyFile.write(received_strategy.data(), received_strategy.size());
            strategyFile.close();
        }
        else
        {
            Common::logWarning("Invalid \"strategy.ims\" received with size: {}", received_strategy.size());
        }
    }
}
} // namespace Tyr::Cli
