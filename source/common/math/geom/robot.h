#pragma once

namespace Immortals::Common
{
struct Robot
{
    Robot(const Vec2 t_center, const float t_radius, const Angle t_angle)
        : center(t_center), radius(t_radius), angle(t_angle)
    {}

    // distance between front face of the robot to the center
    float frontDis() const
    {
        return radius * kHalfArcAngle.cos();
    }

    bool inside(const Vec2 t_point) const
    {
        const Vec2  rel = t_point - center;
        const float dis = rel.length();

        const Angle start_angle = angle - kHalfArcAngle;
        const Angle end_angle   = angle + kHalfArcAngle;

        return dis < frontDis() || (dis <= radius && rel.toAngle().isBetween(start_angle, end_angle));
    }

    bool canKick(const Vec2 t_point, const float t_kicker_depth = kKickerDepth) const
    {
        const Vec2 p1 = center + (angle + kHalfArcAngle).toUnitVec() * radius - angle.toUnitVec() * t_kicker_depth * 0.5f;
        const Vec2 p2 = center + (angle - kHalfArcAngle).toUnitVec() * radius - angle.toUnitVec() * t_kicker_depth * 0.5f;
        const Vec2 p3 = center + (angle - kHalfArcAngle).toUnitVec() * radius + angle.toUnitVec() * t_kicker_depth;
        const Vec2 p4 = center + (angle + kHalfArcAngle).toUnitVec() * radius + angle.toUnitVec() * t_kicker_depth;


        const Vec2 v1 = t_point - p1;
        const Vec2 v2 = t_point - p2;
        const Vec2 v3 = t_point - p3;
        const Vec2 v4 = t_point - p4;

        const float cross1 = (p2 - p1).cross(v1);
        const float cross2 = (p3 - p2).cross(v2);
        const float cross3 = (p4 - p3).cross(v3);
        const float cross4 = (p1 - p4).cross(v4);

        return signInt(cross1) == signInt(cross2) && signInt(cross2) == signInt(cross3) &&
               signInt(cross3) == signInt(cross4);
    }

    void getFrontPoints(Vec2 &t_p1, Vec2 &t_p2, Vec2 &t_p3, Vec2 &t_p4, const float t_kicker_depth = kKickerDepth) const
    {
        t_p1 = center + (angle + kHalfArcAngle).toUnitVec() * radius - angle.toUnitVec() * t_kicker_depth * 0.5f;
        t_p2 = center + (angle - kHalfArcAngle).toUnitVec() * radius -angle.toUnitVec() * t_kicker_depth * 0.5f;
        t_p3 = center + (angle - kHalfArcAngle).toUnitVec() * radius + angle.toUnitVec() * t_kicker_depth;
        t_p4 = center + (angle + kHalfArcAngle).toUnitVec() * radius + angle.toUnitVec() * t_kicker_depth;
    }

    LineSegment getFrontLine() const
    {
        const Vec2 p1 = center + (angle + kHalfArcAngle).toUnitVec() * radius;
        const Vec2 p2 = center + (angle - kHalfArcAngle).toUnitVec() * radius;
        return LineSegment(p1, p2);
    }

    static const inline Angle kHalfArcAngle = Angle::fromDeg(50.0f);
    static constexpr float    kKickerDepth  = 150.0f;

    Vec2  center;
    float radius;
    Angle angle;
};
} // namespace Immortals::Common
