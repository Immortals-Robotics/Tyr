#pragma once

#include "sender.h"

namespace Tyr::Sender
{
class Nrf final : public Base
{
private:
    std::shared_ptr<Common::UdpServer> commUDP;

    void appendData(unsigned char *data, int length);
    void append_demo_data();

    void queueCommand(const Command &command);

public:
    int buff_idx;
    int startup;

    Nrf();

    bool send() override;
};
} // namespace Tyr::Sender
