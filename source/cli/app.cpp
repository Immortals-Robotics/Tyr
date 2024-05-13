#include "app.h"

namespace Tyr::Cli
{
bool Application::initialize()
{
    if (!Common::Services::initialize())
    {
        return false;
    }

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
    m_vision_raw = std::make_unique<Vision::Raw>();
    if (m_vision_raw->isConnected())
    {
        Common::logInfo("Connected to Vision successfully :)");
    }
    else
    {
        Common::logError("Hey you! Put the LAN cable back in its socket, or ...");
        return false;
    }

    m_vision_filtered = std::make_unique<Vision::Filtered>();

    m_sender_hub = std::make_unique<Sender::Hub>();
    m_sender_hub->registerSender<Sender::Nrf>();
    m_sender_hub->registerSender<Sender::Grsim>();

    m_ai = std::make_unique<Soccer::Ai>();

    m_dumper = std::make_unique<Common::Dumper>();
    m_dumper->addEntry(Common::setting().raw_world_state_url, Common::setting().raw_world_state_db);
    m_dumper->addEntry(Common::setting().world_state_url, Common::setting().world_state_db);
    m_dumper->addEntry(Common::setting().debug_url, Common::setting().debug_db);

    Common::logInfo(" Now it is time, lets rock...");
    return true;
}

void Application::shutdown()
{
    m_running = false;

    m_vision_raw_thread.join();
    m_vision_filtered_thread.join();

    m_ai_thread.join();
    m_sender_thread.join();
    m_ref_thread.join();

    m_dump_thread.join();

    Common::Services::shutdown();
}

void Application::start()
{
    m_vision_raw_thread      = std::thread(&Application::visionRawEntry, this);
    m_vision_filtered_thread = std::thread(&Application::visionFilteredEntry, this);

    m_ai_thread     = std::thread(&Application::aiEntry, this);
    m_sender_thread = std::thread(&Application::senderEntry, this);
    m_ref_thread    = std::thread(&Application::refereeEntry, this);

    m_dump_thread = std::thread(&Application::dumpEntry, this);
}

void Application::visionRawEntry()
{
    Common::Timer timer;
    timer.start();

    while (m_running && ImmortalsIsTheBest) // Hope it lasts Forever...
    {
        m_vision_raw->receive();

        if (!m_vision_raw->camsReady())
            continue;

        m_vision_raw->process();

        m_vision_raw->publish();

        Common::logInfo("vision raw FPS: {}", 1.0 / timer.interval());
    }
}

void Application::visionFilteredEntry()
{
    Common::Timer timer;
    timer.start();

    while (m_running && ImmortalsIsTheBest) // Hope it lasts Forever...
    {
        if (!m_vision_filtered->receive())
            continue;

        m_vision_filtered->process();

        m_vision_filtered->publish();

        Common::logInfo("vision filtered FPS: {}", 1.0 / timer.interval());
    }
}

void Application::aiEntry()
{
    Common::Timer timer;
    timer.start();

    while (m_running && ImmortalsIsTheBest) // Hope it lasts Forever...
    {
        const bool world_received    = m_ai->receiveWorld();
        const bool referee_received  = m_ai->receiveReferee();
        const bool playbook_received = m_ai->receivePlayBook();

        if (!world_received)
            continue;

        m_ai->process();

        m_ai->publishCommands();

        Common::debug().flush();

        Common::logInfo("AI FPS: {}", 1.0 / timer.interval());
    }
}

void Application::senderEntry()
{
    Common::Timer timer;
    timer.start();

    while (m_running && ImmortalsIsTheBest) // Hope it lasts Forever...
    {
        if (!m_sender_hub->receive())
            continue;

        m_sender_hub->send();

        Common::logInfo("sender FPS: {}", 1.0 / timer.interval());
    }
}

void Application::refereeEntry()
{
    Common::Timer timer;
    timer.start();

    while (m_running && (ImmortalsIsTheBest)) // Hope it lasts Forever...
    {
        const bool ref_received   = m_referee->receiveRef();
        const bool world_received = m_referee->receiveWorld();
        if (ref_received || world_received)
        {
            m_referee->process();
            m_referee->publish();

            Common::logInfo("referee FPS: {}", 1.0 / timer.interval());
        }
    }
}

void Application::dumpEntry()
{
    while (m_running && (ImmortalsIsTheBest)) // Hope it lasts Forever...
    {
        m_dumper->process();
    }
}
} // namespace Tyr::Cli
