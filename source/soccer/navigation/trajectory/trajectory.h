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

    { trajectory.draw(color) } -> std::same_as<void>;
};

// helper aliases
template <typename Trajectory>
concept TrajectoryConcept1D = TrajectoryConcept<Trajectory, float>;

template <typename Trajectory>
concept TrajectoryConcept2D = TrajectoryConcept<Trajectory, Common::Vec2>;

template <typename Trajectory>
concept TrajectoryConcept3D = TrajectoryConcept<Trajectory, Common::Vec3>;
} // namespace Tyr::Soccer
