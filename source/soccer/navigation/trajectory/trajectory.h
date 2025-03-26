#pragma once

namespace Tyr::Soccer
{
// Concept defining required interface for trajectory types
template <typename Trajectory, typename T>
concept TrajectoryConcept = requires(const Trajectory& trajectory, float t, Common::Color color)
{
    { trajectory.getPosition(t) } -> std::convertible_to<T>;
    { trajectory.getVelocity(t) } -> std::convertible_to<T>;
    { trajectory.getAcceleration(t) } -> std::convertible_to<T>;

    { trajectory.getStartTime() } -> std::convertible_to<float>;
    { trajectory.getEndTime() } -> std::convertible_to<float>;
    { trajectory.getDuration() } -> std::convertible_to<float>;
};

// helper aliases
template <typename Trajectory>
concept TrajectoryConcept1D = TrajectoryConcept<Trajectory, float>;

template <typename Trajectory>
concept TrajectoryConcept2D = TrajectoryConcept<Trajectory, Common::Vec2>;

template <typename Trajectory>
concept TrajectoryConcept3D = TrajectoryConcept<Trajectory, Common::Vec3>;

// utility
template <TrajectoryConcept2D Trajectory>
void drawTrajectory(const Trajectory& trajectory, const Common::Color color)
{
    static constexpr float dt = 0.1f;
    for (float t = trajectory.getStartTime(); t < trajectory.getEndTime() - dt; t += dt)
    {
        const Common::Vec2 p0 = trajectory.getPosition(t);
        const Common::Vec2 p1 = trajectory.getPosition(t + dt);
        Common::debug().draw(Common::LineSegment{p0, p1}, color);
    }
}

} // namespace Tyr::Soccer
