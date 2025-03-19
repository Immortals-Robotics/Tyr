#include "trajectory_2d.h"
#include "trajectory_pieced.h"

#include "../../robot/velocity_profile.h"

namespace Tyr::Soccer
{
// note: these are basically copy-pasted from tigers' code-basse.

float Trajectory2D::velChangeToZero(const float s0, const float v0, const float aMax)
{
    const float a = 0 >= v0 ? aMax : -aMax;

    const float t = -v0 / a;
    return s0 + (0.5f * v0 * t);
}

float Trajectory2D::velTriToZero(const float s0, const float v0, const float v1, const float aMax)
{
    float a1;
    float a2;

    if (v1 >= v0)
    {
        a1 = aMax;
        a2 = -aMax;
    } else
    {
        a1 = -aMax;
        a2 = aMax;
    }

    const float t1 = (v1 - v0) / a1;
    const float s1 = s0 + (0.5f * (v0 + v1) * t1);

    const float t2 = -v1 / a2;
    return s1 + (0.5f * v1 * t2);
}

Trajectory2D::Trajectory1D Trajectory2D::calcTri(const float s0, const float v0, const float s2, const float a)
{
    float t2;
    float v1;
    float t1;
    float s1;
    float sq;

    if (a > 0)
    {
        // + -
        sq = ((a * (s2 - s0)) + (0.5f * v0 * v0)) / (a * a);
    } else
    {
        // - +
        sq = ((-a * (s0 - s2)) + (0.5f * v0 * v0)) / (a * a);
    }

    if (sq > 0.0f)
    {
        t2 = std::sqrt(sq);
    } else
    {
        t2 = 0;
    }
    v1 = a * t2;
    t1 = (v1 - v0) / a;
    s1 = s0 + ((v0 + v1) * 0.5f * t1);

    TrajectoryPiece1D piece_0{};
    piece_0.t_end = t1;
    piece_0.acc = a;
    piece_0.v_start = v0;
    piece_0.p_start = s0;

    TrajectoryPiece1D piece_1{};
    piece_1.t_start = t1;
    piece_1.t_end = t1 + t2;
    piece_1.acc = -a;
    piece_1.v_start = v1;
    piece_1.p_start = s1;

    Trajectory1D trajectory{};
    trajectory.addPiece(piece_0);
    trajectory.addPiece(piece_1);

    return trajectory;
}

Trajectory2D::Trajectory1D Trajectory2D::calcTrapz(
            const float s0,
            const float v0,
            const float v1,
            const float s3,
            const float aMax
    )
{
    float a1;
    float a3;
    float t1;
    float t2;
    float t3;
    float v2;
    float s1;
    float s2;

    if (v0 > v1)
    {
        a1 = -aMax;
    } else
    {
        a1 = aMax;
    }

    if (v1 > 0)
    {
        a3 = -aMax;
    } else
    {
        a3 = aMax;
    }

    t1 = (v1 - v0) / a1;
    v2 = v1;
    t3 = -v2 / a3;

    s1 = s0 + (0.5f * (v0 + v1) * t1);
    s2 = s3 - (0.5f * v2 * t3);
    t2 = (s2 - s1) / v1;

    TrajectoryPiece1D piece_0{};
    piece_0.t_end = t1;
    piece_0.acc = a1;
    piece_0.v_start = v0;
    piece_0.p_start = s0;

    TrajectoryPiece1D piece_1{};
    piece_1.t_start = t1;
    piece_1.t_end = t1 + t2;
    piece_1.acc = 0;
    piece_1.v_start = v1;
    piece_1.p_start = s1;

    TrajectoryPiece1D piece_2{};
    piece_2.t_start = t1 + t2;
    piece_2.t_end = t1 + t2 + t3;
    piece_2.acc = a3;
    piece_2.v_start = v2;
    piece_2.p_start = s2;

    Trajectory1D trajectory{};
    trajectory.addPiece(piece_0);
    trajectory.addPiece(piece_1);
    trajectory.addPiece(piece_2);

    return trajectory;
}

Trajectory2D::Trajectory1D Trajectory2D::makeBangBangTrajectory1D(
    const float initialPos,
    const float finalPos,
    const float initialVel,
    const float maxVel,
    const float maxAcc)
{
    float x0 = initialPos;
    float xd0 = initialVel;
    float xTrg = finalPos;
    float xdMax = maxVel;
    float xddMax = maxAcc;
    float sAtZeroAcc = velChangeToZero(x0, xd0, xddMax);

    if (sAtZeroAcc <= xTrg)
    {
        float sEnd = velTriToZero(x0, xd0, xdMax, xddMax);

        if (sEnd >= xTrg)
        {
            // Triangular profile
            return calcTri(x0, xd0, xTrg, xddMax);
        } else
        {
            // Trapezoidal profile
            return calcTrapz(x0, xd0, xdMax, xTrg, xddMax);
        }
    } else
    {
        // even with a full brake we miss xTrg
        float sEnd = velTriToZero(x0, xd0, -xdMax, xddMax);

        if (sEnd <= xTrg)
        {
            // Triangular profile
            return calcTri(x0, xd0, xTrg, -xddMax);
        } else
        {
            // Trapezoidal profile
            return calcTrapz(x0, xd0, -xdMax, xTrg, xddMax);
        }
    }
}

Trajectory2D Trajectory2D::makeBangBangTrajectory(const Common::Vec2 s0, const Common::Vec2 v0, const Common::Vec2 s1, const VelocityProfile &profile)
{
    // alphaFn is either
    // - [async] alpha -> alpha + (((float) AngleMath.PI_HALF - alpha) * 0.5f);
    // - [sync] f -> f
    const float accuracy = 1e-3f;

    const float vmax = profile.speed;
    const float acc = profile.acceleration;

    const float s0x = s0.x;
    const float s0y = s0.y;
    const float s1x = s1.x;
    const float s1y = s1.y;
    const float v0x = v0.x;
    const float v0y = v0.y;

    Trajectory2D trajectory{};

    Common::Angle inc = Common::Angle::fromRad(std::numbers::pi_v<float> / 8.0f);
    Common::Angle alpha = Common::Angle::fromRad(std::numbers::pi_v<float> / 4.0f);

    // binary search, some iterations (fixed)
    while (inc.rad() > 1e-7)
    {
        const float sA = alpha.sin();
        const float cA = alpha.cos();

        trajectory.m_trajectory_x = makeBangBangTrajectory1D(s0x, s1x, v0x, vmax * cA, acc * cA);
        trajectory.m_trajectory_y = makeBangBangTrajectory1D(s0y, s1y, v0y, vmax * sA, acc * sA);

        const float diff = std::abs(trajectory.m_trajectory_x.getDuration() - trajectory.m_trajectory_y.getDuration());
        if (diff < accuracy)
        {
            break;
        }
        if (trajectory.m_trajectory_x.getDuration() > trajectory.m_trajectory_y.getDuration())
        {
            alpha -= inc;
        } else
        {
            alpha += inc;
        }

        inc = inc * 0.5f;
    }

    return trajectory;
}
} // namespace Tyr::Soccer
