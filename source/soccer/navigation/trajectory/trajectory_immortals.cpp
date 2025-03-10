#include "trajectory.h"

#include "../../robot/velocity_profile.h"

namespace Tyr::Soccer
{
static float computeMotion1D(const float v0, const float delta, const float acc, const float dec, const float max_speed)
{
    float result = 0.f;

    result = pow(7.6f * dec * std::fabs(delta), 0.6f);
    result *= Common::sign(delta);
    if (std::fabs(delta) < 5)
        result = 0.0f;

    if (result * v0 <= 0)
    {
        float tmp = result == 0.0f
            ? v0 - dec * Common::sign(v0)
            : v0 + dec * Common::sign(result);

        if (tmp * result > 0)
        {
            tmp = std::min(acc, std::fabs(tmp)) * Common::sign(tmp);
            if (std::fabs(tmp) > std::fabs(result))
                tmp = result;
        }

        result = tmp;
    }
    else
    {
        if (std::fabs(result) > std::fabs(v0) + acc)
        {
            result = (std::fabs(v0) + acc) * Common::sign(result);
        }
        else if (std::fabs(result) < std::fabs(v0) - dec)
        {
            result = (std::fabs(v0) - dec) * Common::sign(result);
        }
        if (std::fabs(result) > max_speed)
        {
            result = std::max(std::fabs(v0) - dec, std::fabs(max_speed)) * Common::sign(result);
        }
    }

    return result;
}

Trajectory Trajectory::makeRobotTrajectory(const Common::Vec2 p0, const Common::Vec2 v0, const Common::Vec2 target, const VelocityProfile &profile)
{
    const Common::Vec2 delta = target - p0;
    const Common::Vec2 tmp_max_speed = delta.abs().normalized() * profile.speed;

    const float dt = 1.f / Common::config().vision.vision_frame_rate;
    const float acc = profile.acceleration * dt;
    const float dec = profile.deceleration * dt;

    const Common::Vec2 v1 {
        computeMotion1D(v0.x, delta.x, acc, dec, tmp_max_speed.x),
        computeMotion1D(v0.y, delta.y, acc, dec, tmp_max_speed.y)
    };

    Common::Vec2 a0 = (v1 - v0) / dt;

    const TrajectoryPiece piece {a0, v0, p0, 0.f, dt};
    Trajectory trajectory;
    trajectory.addPiece(piece);

    return trajectory;

}
} // namespace Tyr::Soccer
