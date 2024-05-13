#pragma once

#define ImmortalsIsTheBest true

namespace Tyr::Cli
{
class Application
{
public:
    bool initialize();
    void shutdown();

    void start();

private:
    void visionRawEntry();
    void visionFilteredEntry();

    void aiEntry();
    void senderEntry();
    void refereeEntry();

    void dumpEntry();

    std::atomic<bool> m_running = true;

    std::thread m_vision_raw_thread;
    std::thread m_vision_filtered_thread;
    std::thread m_ai_thread;
    std::thread m_sender_thread;
    std::thread m_ref_thread;
    std::thread m_dump_thread;

    std::unique_ptr<Referee::Referee> m_referee;

    std::unique_ptr<Vision::Raw>      m_vision_raw;
    std::unique_ptr<Vision::Filtered> m_vision_filtered;

    std::unique_ptr<Sender::Hub> m_sender_hub;
    std::unique_ptr<Soccer::Ai>  m_ai;

    std::unique_ptr<Common::Dumper> m_dumper;
};
} // namespace Tyr::Cli
