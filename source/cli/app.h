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
    void aiEntry();
    void refereeEntry();
    void strategyEntry();

    std::mutex        m_lock;
    std::atomic<bool> m_exited = false;

    std::thread m_ai_thread;
    std::thread m_ref_thread;
    std::thread m_str_thread;

    std::unique_ptr<Referee::Referee> m_referee;

    std::unique_ptr<Vision::Vision> m_vision;
    std::vector<std::unique_ptr<Sender::Base>> m_senders;
    std::unique_ptr<Soccer::Ai>     m_ai;

	std::unique_ptr<Common::UdpClient> m_strategy_udp;
};
} // namespace Tyr::Cli
