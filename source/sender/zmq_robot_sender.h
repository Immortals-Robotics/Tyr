#pragma once

#include <array>

#include "base.h"

namespace Tyr::Sender
{
class ZmqRobotSender final : public Base
{
public:
    ZmqRobotSender();
    ~ZmqRobotSender() override;

    bool send(const CommandsWrapper &t_wrapper) override;

private:
    void *m_context = nullptr;
    std::array<void *, Common::Config::Common::kMaxRobots> m_sockets{};

    void *socketForRobot(int t_robot_id);

    template <typename Message>
    bool sendTopic(void *t_socket, uint8_t t_topic, const Message &t_message);
};
} // namespace Tyr::Sender
