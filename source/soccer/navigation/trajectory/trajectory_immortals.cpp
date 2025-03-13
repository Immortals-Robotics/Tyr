#include "trajectory_2d.h"

#include "../../robot/velocity_profile.h"

namespace Tyr::Soccer
{
static float computeMotion1D(const float v0, const float delta, const float acc, const float max_speed)
{
    float result = 0.f;

    result = pow(7.6f * acc * std::fabs(delta), 0.6f);
    result *= Common::sign(delta);
    if (std::fabs(delta) < 5)
        result = 0.0f;

    if (result * v0 <= 0)
    {
        float tmp = result == 0.0f
            ? v0 - acc * Common::sign(v0)
            : v0 + acc * Common::sign(result);

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
        else if (std::fabs(result) < std::fabs(v0) - acc)
        {
            result = (std::fabs(v0) - acc) * Common::sign(result);
        }
        if (std::fabs(result) > max_speed)
        {
            result = std::max(std::fabs(v0) - acc, std::fabs(max_speed)) * Common::sign(result);
        }
    }

    return result;
}

Trajectory2D Trajectory2D::makeRobotTrajectory(const Common::Vec2 p0, const Common::Vec2 v0, const Common::Vec2 target, const VelocityProfile &profile)
{
    const Common::Vec2 delta = target - p0;
    const Common::Vec2 tmp_max_speed = delta.abs().normalized() * profile.speed;

    const float dt = 1.f / Common::config().vision.vision_frame_rate;
    const float acc = profile.acceleration * dt;

    const Common::Vec2 v1 {
        computeMotion1D(v0.x, delta.x, acc, tmp_max_speed.x),
        computeMotion1D(v0.y, delta.y, acc, tmp_max_speed.y)
    };

    Common::Vec2 a0 = (v1 - v0) / dt;

    TrajectoryPiece2D piece{};
    piece.acc = a0;
    piece.v_start = v0;
    piece.p_start = p0;
    piece.t_end = dt;

    Trajectory2D trajectory;
    trajectory.addPiece(piece);

    return trajectory;

}
} // namespace Tyr::Soccer
