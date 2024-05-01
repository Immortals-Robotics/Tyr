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
    void aiThreadEentry();
    void refereeThreadEentry();
    void strategyThreadEentry();

    std::mutex        m_lock;
    std::atomic<bool> m_exited = false;

    std::thread m_ai_thread;
    std::thread m_ref_thread;
    std::thread m_str_thread;

    std::unique_ptr<Common::WorldState>   m_world_state;
    std::unique_ptr<Common::RefereeState> m_referee_state;

    std::unique_ptr<Referee::Referee> m_referee;

    std::unique_ptr<Vision::Vision> m_vision;
    std::unique_ptr<Sender::Sender> m_sender;
    std::unique_ptr<Soccer::Ai>     m_ai;
    std::unique_ptr<Sender::Grsim>  m_grsim;
};
} // namespace Tyr::Cli
