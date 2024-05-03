#pragma once

#include "command.h"

namespace Tyr::Sender
{
class Sender
{
private:
    std::shared_ptr<Common::UdpServer> commUDP;

public:
    int buff_idx;
    int startup;

    void getCommand(const Command& command);
    bool appendData(unsigned char *data, int length);
    bool sendAll();
    void append_demo_data();

    Sender();
};
} // namespace Tyr::Sender
