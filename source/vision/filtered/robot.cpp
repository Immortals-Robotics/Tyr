#include "filtered.h"

namespace Tyr::Vision
{
void Filtered::processRobots(const Common::TeamColor t_color)
{
    const int color_id = static_cast<int>(t_color);

    auto &raw_robots = t_color == Common::TeamColor::Yellow
        ? m_raw_state.yellow_robots
        : m_raw_state.blue_robots;

    const bool own = Common::config().common.our_color == t_color;

    auto &robots = own
        ? m_state.own_robot
        : m_state.opp_robot;

    for (int id = 0; id < Common::Config::Common::kMaxRobots; id++)
    {
        auto &robot = robots[id];

        robot.color     = t_color;
        robot.vision_id = id;

        TrackedRobot& tracked_robot = m_tracked_robot[color_id][id];

        tracked_robot.predict();

        // find the corresponding raw robot
        int raw_robot_id = -1;
        for (size_t raw_id = 0; raw_id < raw_robots.size(); raw_id++)
        {
            const auto &raw_robot = raw_robots[raw_id];

            if (raw_robot.id == id)
            {
                raw_robot_id = raw_id;
                break;
            }
        }

        // and update the kalman based on it
        if (raw_robot_id != -1)
        {
            const auto &raw_robot = raw_robots[raw_robot_id];

            if (robot.seen_state == Common::SeenState::CompletelyOut)
            {
                tracked_robot.reset(raw_robot.position, raw_robot.angle);
            }

            tracked_robot.update(raw_robot.position, raw_robot.angle);
        }

        // predict
        const Filter::RobotState predicted_state = own
            ? tracked_robot.predictOwn(kPredictTime, m_cmd_map[id])
            : tracked_robot.predictOpp(kPredictTime);

        // fill the robot state based on kalman
        robot.position = predicted_state.position();
        robot.velocity = predicted_state.velocity();
        robot.angle = predicted_state.angle();
        robot.angular_velocity =  predicted_state.angularVelocity();

        if (tracked_robot.notSeen() == 0)
        {
            robot.seen_state = Common::SeenState::Seen;
        }
        else if (tracked_robot.notSeen() < Common::config().vision.max_robot_frame_not_seen)
        {
            robot.seen_state = Common::SeenState::TemporarilyOut;
        }
        else
        {
            robot.seen_state = Common::SeenState::CompletelyOut;
        }

        if (tracked_robot.notSeen() < kMaxRobotSubstitute)
        {
            robot.out_for_substitute = false;
        }
        else
        {
            robot.out_for_substitute = true;
        }
    }
}
} // namespace Tyr::Vision
