#include "grsim.h"

namespace Tyr::Sender
{
Grsim::Grsim(Common::NetworkAddress address) : address(address)
{
    socket = std::make_unique<Common::UdpServer>();
}

#if GRSIM_FIXED
void Grsim::SendData(const Robot *const robots, const int robot_count, bool color)
{
    Protos::grSim_Packet packet;
    packet.mutable_commands()->set_isteamyellow(color);
    packet.mutable_commands()->set_timestamp(0.0);

    for (int robot_idx = 0; robot_idx < robot_count; ++robot_idx)
    {
        const Robot *const robot = robots + robot_idx;

        Protos::grSim_Robot_Command *command = packet.mutable_commands()->add_robot_commands();
        command->set_id(robot->vision_id);

        command->set_wheelsspeed(false);
        /*command->set_wheel1(edtV1->text().toDouble());
        command->set_wheel2(edtV2->text().toDouble());
        command->set_wheel3(edtV3->text().toDouble());
        command->set_wheel4(edtV4->text().toDouble());*/

        const int          cmd_idx   = robots[robot_idx].lastCMDs[10].x;
        const Common::Vec3 motion    = robots[robot_idx].lastCMDs[cmd_idx];
        float              robot_ang = (90 - robot->State.Angle) * 3.1415 / 180.0;
        float              new_VelX  = motion.x * cos(robot_ang) - motion.y * sin(robot_ang);
        float              new_VelY  = motion.x * sin(robot_ang) + motion.y * cos(robot_ang);

        command->set_veltangent(new_VelY / 20.0);
        command->set_velnormal(-new_VelX / 20.0);

        float w = robot->target.Angle - robot->State.Angle;
        while (w > 180)
        {
            w -= 360;
        }
        while (w < -180)
        {
            w += 360;
        }
        w /= 10.0f;

        command->set_velangular(w);
        // command->set_velangular(0);

        if (robot->shoot > 0)
        {
            command->set_kickspeedx(robot->shoot / 10.f);
            command->set_kickspeedz(0);
        }
        else if (robot->chip > 0)
        {
            float chip = 0.f; // robot->chip / 25.0f;
            command->set_kickspeedx(chip * 0.707f);
            command->set_kickspeedz(chip / 0.707f);
        }
        else
        {
            command->set_kickspeedx(.0f);
            command->set_kickspeedz(.0f);
        }

        command->set_spinner(robot->dribbler);

        socket->send(packet, address);
    }
}
#endif
} // namespace Tyr::Sender