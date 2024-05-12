#include "nrf.h"

#include "protocol/writer.h"

namespace Tyr::Sender
{
Nrf::Nrf() : Base()
{
    commUDP = std::make_shared<Common::UdpServer>();

    buff_idx    = 0;
    auto buffer = commUDP->getBuffer();
    for (int i = 0; i < buffer.size(); i++)
    {
        buffer[i] = 0x00;
    }
    startup = 5;
}

void Nrf::queueCommand(const Command &command)
{
    unsigned char data[32] = {};

    Common::Vec3 motion = command.motion;
    motion.x *= 2.55;
    motion.y *= 2.55;
    convert_float_to_2x_buff(data + 3, motion.x);
    convert_float_to_2x_buff(data + 5, motion.y);
    convert_float_to_2x_buff(data + 7, command.target_angle.deg());

    data[0] = command.vision_id;
    if (command.halted)
    {
        data[1] = 0x0A; // length=10
        data[2] = 0x06; // Command to HALT
        data[3] = 0x00;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        data[8] = 0x00;
        data[9] = 0x00;
    }
    else
    {
        data[1] = 15; // length=10
        data[2] = 12; // Command to move with new protocol

        convert_float_to_2x_buff(data + 9, command.current_angle.deg());
        if (command.shoot > 0)
        {
            data[11] = command.shoot;
            data[12] = 0x00;
        }
        else if (command.chip > 0)
        {
            data[11] = 0x00;
            data[12] = command.chip;
        }
        else
        {
            data[11] = 0x00;
            data[12] = 0x00;
        }
    }

    appendData(data, data[1]);
}

void Nrf::appendData(unsigned char *data, int length)
{
    auto buffer = commUDP->getBuffer();

    for (int i = 0; i < length; i++)
    {
        buffer[i + buff_idx] = data[i];
    }
    buff_idx += length;
}

bool Nrf::send()
{
    if (startup > 0)
    {
        startup--;
        return false;
    }

    for (const auto &command : m_wrapper.command)
        queueCommand(command);

    append_demo_data();

    const bool result = commUDP->send(
        buff_idx, Common::NetworkAddress{Common::setting().sender_address.ip, Common::setting().sender_address.port});

    if (!result)
    {
        Common::logError("ERROR: failed to send robot packets.");
    }

    buff_idx = 0;
    return result;
}

void Nrf::append_demo_data()
{
    unsigned char data[10];
    data[0] = 25;
    data[1] = 0x0A;
    data[2] = 0x00;
    data[3] = 0x00;
    data[4] = 0x00;
    data[5] = 0x00;
    data[6] = 0x00;
    data[7] = 0x00;
    data[8] = 0x00;
    data[9] = 0x00;

    appendData(data, 10);
}
} // namespace Tyr::Sender
