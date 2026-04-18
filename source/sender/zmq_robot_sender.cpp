#include "zmq_robot_sender.h"

#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

#include <zmq.h>

#include "zmq_protocol.pb.h"

namespace Tyr::Sender
{
namespace
{
constexpr int kCmdPort = 5671;
constexpr float kDefaultDribblerForce = 5.0f;
constexpr int kSendTimeoutMs = 5;
constexpr int kSendHighWaterMark = 32;
constexpr int kImmediate = 1;

// Populate the robot IPs you want to target. Empty entries are ignored.
constexpr std::array<std::string_view, Common::Config::Common::kMaxRobots> kRobotIps = {
    "", "", "", "", "192.168.188.5", "", "192.168.188.2", "",
    "192.168.188.6", "", "", "", "", "", "", "",
};

std::string endpointForRobot(const int t_robot_id)
{
    return std::string("tcp://") + std::string(kRobotIps[t_robot_id]) + ":" + std::to_string(kCmdPort);
}

float getShootPower(const Command &t_command)
{
    if (t_command.shoot <= 0.0f)
    {
        return 0.0f;
    }

    return std::clamp(t_command.shoot, 0.0f, 6500.0f);
}
} // namespace

ZmqRobotSender::ZmqRobotSender()
{
    m_context = zmq_ctx_new();
    if (m_context == nullptr)
    {
        Common::logError("Failed to create ZMQ context: {}", zmq_strerror(zmq_errno()));
        return;
    }

    for (int robot_id = 0; robot_id < static_cast<int>(kRobotIps.size()); ++robot_id)
    {
        if (kRobotIps[robot_id].empty())
        {
            continue;
        }

        socketForRobot(robot_id);
    }
}

ZmqRobotSender::~ZmqRobotSender()
{
    for (auto &socket : m_sockets)
    {
        if (socket == nullptr)
        {
            continue;
        }

        const int linger = 0;
        zmq_setsockopt(socket, ZMQ_LINGER, &linger, sizeof(linger));
        zmq_close(socket);
        socket = nullptr;
    }

    if (m_context != nullptr)
    {
        zmq_ctx_destroy(m_context);
        m_context = nullptr;
    }
}

void *ZmqRobotSender::socketForRobot(const int t_robot_id)
{
    if (t_robot_id < 0 || t_robot_id >= static_cast<int>(m_sockets.size()))
    {
        return nullptr;
    }

    if (m_context == nullptr || kRobotIps[t_robot_id].empty())
    {
        return nullptr;
    }

    if (m_sockets[t_robot_id] != nullptr)
    {
        return m_sockets[t_robot_id];
    }

    void *socket = zmq_socket(m_context, ZMQ_PUSH);
    if (socket == nullptr)
    {
        Common::logError("Failed to create ZMQ PUSH socket for robot {}: {}", t_robot_id, zmq_strerror(zmq_errno()));
        return nullptr;
    }

    const int linger = 0;
    zmq_setsockopt(socket, ZMQ_LINGER, &linger, sizeof(linger));
    zmq_setsockopt(socket, ZMQ_SNDTIMEO, &kSendTimeoutMs, sizeof(kSendTimeoutMs));
    zmq_setsockopt(socket, ZMQ_SNDHWM, &kSendHighWaterMark, sizeof(kSendHighWaterMark));
    zmq_setsockopt(socket, ZMQ_IMMEDIATE, &kImmediate, sizeof(kImmediate));

    const std::string endpoint = endpointForRobot(t_robot_id);
    if (zmq_connect(socket, endpoint.c_str()) != 0)
    {
        Common::logError("Failed to connect ZMQ PUSH socket for robot {} to {}: {}",
                         t_robot_id,
                         endpoint,
                         zmq_strerror(zmq_errno()));
        zmq_close(socket);
        return nullptr;
    }

    Common::logInfo("Connected ZMQ command stream for robot {} to {}", t_robot_id, endpoint);
    m_sockets[t_robot_id] = socket;
    return socket;
}

bool ZmqRobotSender::send(const CommandsWrapper &t_wrapper)
{
    if (m_context == nullptr)
    {
        return false;
    }

    bool any_sent = false;
    for (const auto &command : t_wrapper.commands)
    {
        void *socket = socketForRobot(command.vision_id);
        if (socket == nullptr)
        {
            continue;
        }

        if (command.halted)
        {
            continue;
        }

        immortals::MikonaCommand mikona_command;
        mikona_command.set_charge(!command.halted);
        mikona_command.set_discharge(command.halted);

        immortals::RobotCommand robot_command;
        robot_command.set_halted(command.halted);
        robot_command.set_vx(command.motion.x * 1.2f / 1000.0f);
        robot_command.set_vy(command.motion.y * 1.2f / 1000.0f);
        robot_command.set_current_angle(command.current_angle.deg());
        robot_command.set_target_angle(command.target_angle.deg());
        robot_command.set_chip(getShootPower(command) * 0.7f / 1000.0f);
        robot_command.set_shoot((command.chip > 10 && getShootPower(command) == 0) ? 5.0f : 0.0f);
        robot_command.set_dribbler_speed(command.dribbler);
        robot_command.set_dribbler_force(command.dribbler > 0.0f ? kDefaultDribblerForce : 0.0f);

        const bool mikona_sent =
            sendTopic(socket, static_cast<uint8_t>(immortals::COMMAND_TYPE_MIKONA), mikona_command);
        const bool robot_sent =
            sendTopic(socket, static_cast<uint8_t>(immortals::COMMAND_TYPE_ROBOT), robot_command);

        any_sent = any_sent || mikona_sent || robot_sent;
    }

    return any_sent;
}

template <typename Message>
bool ZmqRobotSender::sendTopic(void *const t_socket, const uint8_t t_topic, const Message &t_message)
{
    const size_t message_size = t_message.ByteSizeLong();
    std::vector<std::byte> buffer(message_size);

    if (!t_message.SerializeToArray(buffer.data(), static_cast<int>(buffer.size())))
    {
        Common::logError("Failed to serialize ZMQ message on topic {}", t_topic);
        return false;
    }

    if (zmq_send(t_socket, &t_topic, sizeof(t_topic), ZMQ_SNDMORE) < 0)
    {
        const int error = zmq_errno();
        if (error == EAGAIN)
        {
            Common::logWarning("Timed out sending ZMQ topic frame {}", t_topic);
        }
        else
        {
            Common::logError("Failed to send ZMQ topic frame {}: {}", t_topic, zmq_strerror(error));
        }
        return false;
    }

    if (zmq_send(t_socket, buffer.data(), buffer.size(), 0) < 0)
    {
        const int error = zmq_errno();
        if (error == EAGAIN)
        {
            Common::logWarning("Timed out sending ZMQ payload frame {}", t_topic);
        }
        else
        {
            Common::logError("Failed to send ZMQ payload frame {}: {}", t_topic, zmq_strerror(error));
        }
        return false;
    }

    return true;
}

template bool ZmqRobotSender::sendTopic<immortals::MikonaCommand>(void *, uint8_t, const immortals::MikonaCommand &);
template bool ZmqRobotSender::sendTopic<immortals::RobotCommand>(void *, uint8_t, const immortals::RobotCommand &);
} // namespace Tyr::Sender
