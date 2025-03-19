#pragma once

namespace Tyr::Soccer
{
struct VelocityProfile;

template<typename T>
class Trajectory
{
public:
    Trajectory() = default;
    virtual ~Trajectory() = default;

    virtual T getPosition(float t) const = 0;
    virtual T getVelocity(float t) const = 0;
    virtual T getAcceleration(float t) const = 0;

    virtual float getStartTime() const = 0;
    virtual float getEndTime() const = 0;

    virtual void draw(Common::Color color) const
    {
        (void)color;
    }

    float getDuration() const
    {
        return std::max(0.0f, getEndTime() - getStartTime());
    }
};
} // namespace Tyr::Soccer
