#include "nrf.h"

#include "protocol/writer.h"

namespace Tyr::Sender
{

static Common::Vec2 shoot_coeffs[Common::Config::Common::kMaxRobots] = {
    {4.45f, 2.59f},  //  0
    {4.48f, 2.6f},   //  1
    {3.42f, 3.43f},  //  2
    {6.22f, -0.43f}, //  3
    {9.74f, 12.8f},  //  4
    {4.46f, 2.66f},  //  5
    {9.74f, 12.8f},  //  6
    {4.02f, 3.38f},  //  7
    {1.0f, 0.0f},    //  8*
    {1.0f, 0.0f},    //  9*
    {1.0f, 0.0f},    // 10*
    {1.0f, 0.0f},    // 11*
    {1.0f, 0.0f},    // 12*
    {1.0f, 0.0f},    // 13*
    {1.0f, 0.0f},    // 14*
    {1.0f, 0.0f},    // 15*
};

static Common::Vec2 chip_coeffs[Common::Config::Common::kMaxRobots] = {
    {1.00000f, 2.32507f}, //  0
    {0.77461f, 2.81861f}, //  1
    {1.69062f, 1.54292f}, //  2
    {0.90561f, 3.13007f}, //  3
    {1.01367f, 1.74667f}, //  4
    {1.01367f, 1.74667f}, //  5
    {1.45657f, 1.18770f}, //  6
    {1.01367f, 1.74667f}, //  7
    {1.0f, 0.0f},         //  8*
    {1.0f, 0.0f},         //  9*
    {1.0f, 0.0f},         // 10*
    {1.0f, 0.0f},         // 11*
    {1.0f, 0.0f},         // 12*
    {1.0f, 0.0f},         // 13*
    {1.0f, 0.0f},         // 14*
    {1.0f, 0.0f},         // 15*
};

static float getCalibratedShootPow(float t_raw_shoot, const Common::Vec2 &t_coeffs)
{
    if (t_raw_shoot <= 0)
    {
        return 0;
    }

    float calib_shoot = t_coeffs.x * t_raw_shoot + t_coeffs.y;

    calib_shoot = std::clamp(calib_shoot, 0.0f, 100.0f);

    return calib_shoot;
}

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

    // TODO: verify this magic number. motion is in mm/s
    convert_float_to_2x_buff(data + 3, command.motion.x / 17.5f);
    convert_float_to_2x_buff(data + 5, command.motion.y / 17.5f);
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
            const float raw_shoot        = std::clamp(command.shoot, 0.0f, 6500.f) / 1000.f;
            const float calibrated_shoot = getCalibratedShootPow(raw_shoot, shoot_coeffs[command.vision_id]);

            data[11] = static_cast<unsigned char>(calibrated_shoot);
            data[12] = 0x00;
        }
        else if (command.chip > 0)
        {
            const float raw_chip        = std::clamp(command.chip, 0.0f, 150.f);
            const float calibrated_chip = getCalibratedShootPow(raw_chip, chip_coeffs[command.vision_id]);

            data[11] = 0x00;
            data[12] = static_cast<unsigned char>(calibrated_chip);
            ;
        }
        else
        {
            data[11] = 0x00;
            data[12] = 0x00;
        }
    }

    appendData(data, data[1]);
}

void Nrf::appendData(unsigned char *data, const int length)
{
    auto buffer = commUDP->getBuffer();

    for (int i = 0; i < length; i++)
    {
        buffer[i + buff_idx] = data[i];
    }
    buff_idx += length;
}

bool Nrf::send(const CommandsWrapper &t_wrapper)
{
    if (startup > 0)
    {
        startup--;
        return false;
    }

    for (const auto &command : t_wrapper.commands)
        queueCommand(command);

    append_demo_data();

    const bool result = commUDP->send(buff_idx, Common::config().network.sender_address);

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
