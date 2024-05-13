#pragma once

#include "base.h"

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

    bool send(const CommandsWrapper &t_wrapper) override;
};
} // namespace Tyr::Sender
