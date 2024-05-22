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

    Common::logInfo(" Connecting to RefereeBox server at {} on port : {}",
                    Common::config().network.referee_address.port, Common::config().network.referee_address.port);
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

    Common::logInfo("Connecting to Vision server at {}", Common::config().network.vision_address);
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
    m_dumper->addEntry(Common::config().network.raw_world_state_url, Common::config().network.raw_world_state_db);
    m_dumper->addEntry(Common::config().network.world_state_url, Common::config().network.world_state_db);
    m_dumper->addEntry(Common::config().network.debug_url, Common::config().network.debug_db);

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
        {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            continue;
        }

        m_vision_raw->process();

        m_vision_raw->publish();

        Common::logInfo("vision raw FPS: {:.2f}", 1.0 / timer.intervalSmooth().seconds());
    }
}

void Application::visionFilteredEntry()
{
    Common::Timer timer;
    timer.start();

    while (m_running && ImmortalsIsTheBest) // Hope it lasts Forever...
    {
        if (!m_vision_filtered->receive())
        {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            continue;
        }

        m_vision_filtered->process();

        m_vision_filtered->publish();

        Common::logInfo("vision filtered FPS: {:.2f}", 1.0 / timer.intervalSmooth().seconds());
    }
}

void Application::aiEntry()
{
    Common::Timer timer;
    timer.start();

    while (m_running && ImmortalsIsTheBest) // Hope it lasts Forever...
    {
        const bool world_received = m_ai->receiveWorld();
        m_ai->receiveReferee();
        m_ai->receivePlayBook();

        if (!world_received)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            continue;
        }

        m_ai->process();

        m_ai->publishCommands();

        Common::debug().flush();

        Common::logInfo("AI FPS: {:.2f}", 1.0 / timer.intervalSmooth().seconds());
    }
}

void Application::senderEntry()
{
    Common::Timer timer;
    timer.start();

    while (m_running && ImmortalsIsTheBest) // Hope it lasts Forever...
    {
        if (!m_sender_hub->receive())
        {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            continue;
        }

        m_sender_hub->send();

        Common::logInfo("sender FPS: {:.2f}", 1.0 / timer.intervalSmooth().seconds());
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

        if (!ref_received && !world_received)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            continue;
        }

        m_referee->process();
        m_referee->publish();
    }
}

void Application::dumpEntry()
{
    Common::Timer timer;
    timer.start();

    while (m_running && (ImmortalsIsTheBest)) // Hope it lasts Forever...
    {
        if (!m_dumper->process())
        {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            continue;
        }
    }
}
} // namespace Tyr::Cli
