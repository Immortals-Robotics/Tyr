#include "../ai.h"

namespace Tyr::Soccer
{
void Ai::halt(const int t_robot_num)
{
    const Common::Vec2 last_cmd = m_own_robot[t_robot_num].currentMotion();

    if (last_cmd.length() > 100.0f)
    {
        const VelocityProfile profile = VelocityProfile::mamooli();
        const float dt = 1.0f / Common::config().vision.vision_frame_rate;
        const Common::Vec2 new_cmd = last_cmd - last_cmd.normalized() * profile.acceleration * dt;

        m_own_robot[t_robot_num].move(new_cmd);
    }
    else
    {
        m_own_robot[t_robot_num].halt();
    }
}

void Ai::haltAll()
{
    for (int i = 0; i < Common::Config::Common::kMaxRobots; i++)
    {
        halt(i);
    }
}
} // namespace Tyr::Soccer
