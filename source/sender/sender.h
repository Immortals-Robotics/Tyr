#pragma once

namespace Tyr::Sender
{
class Sender
{
private:
    std::shared_ptr<Common::UdpServer> commUDP;

public:
    int buff_idx;
    int startup;

    void getCommand(std::span<unsigned char> data);
    bool appendData(unsigned char *data, int length);
    bool sendAll();
    void append_demo_data();

    Sender();
};
} // namespace Tyr::Sender
