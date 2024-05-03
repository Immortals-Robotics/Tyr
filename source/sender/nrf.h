#pragma once

#include "sender.h"

namespace Tyr::Sender
{
class Nrf final : public ISender
{
private:
    std::shared_ptr<Common::UdpServer> commUDP;

    void appendData(unsigned char *data, int length);
    void append_demo_data();

public:
    int buff_idx;
    int startup;

    Nrf();

    void queueCommand(const Command &command) override;
    bool flush() override;
};
} // namespace Tyr::Sender
