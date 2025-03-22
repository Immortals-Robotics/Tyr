#pragma once

#include "../helpers/one_touch_detector.h"
#include "../navigation/planner/planner.h"
#include "velocity_profile.h"

namespace Tyr::Soccer
{
enum class NavigationFlags
{
    None                    = 0,
    NoObstacles        = BIT(1), // only used in ball placement
    BallObstacle       = BIT(2), // 500.0f
    BallMediumObstacle = BIT(3), // 230.0f
    BallSmallObstacle  = BIT(4), // 60.0f
    NoBreak            = BIT(5),
    NoOwnPenaltyArea   = BIT(6),
    NoExtraMargin      = BIT(7),
    BallPlacementLine  = BIT(8),
    TheirHalf          = BIT(9),
};

ENABLE_ENUM_FLAG_OPERATORS(NavigationFlags);

class Robot
{
public:
    Robot() = default;

    Robot(const Robot&) = delete;
    Robot& operator=(const Robot&) = delete;

    Robot(Robot&&) = delete;
    Robot& operator=(Robot&&) = delete;

    void setState(const Common::RobotState *t_state)
    {
        m_state                  = t_state;
        one_touch_detector.robot = t_state;
    }

    const Common::RobotState &state() const
    {
        return *m_state;
    }

    void reset()
    {
        m_shoot     = 0.0f;
        m_chip      = 0.0f;
        m_dribbler  = 0.0f;
        m_navigated = false;
        m_halted    = false;
    }

    void shoot(float pow);
    void chip(float pow);
    void dribble(float pow);

    void navigate(Common::Vec2 t_dest, VelocityProfile t_profile = VelocityProfile::mamooli(),
                  NavigationFlags t_flags = NavigationFlags::None);

    void waitForNavigationJob();

    void move(const Trajectory2D &trajectory);
    void halt();
    void fullBeak(float acc_factor = 1.0f);

    void face(Common::Vec2 t_target);

    [[nodiscard]] float shoot() const
    {
        return m_shoot;
    }

    [[nodiscard]] float chip() const
    {
        return m_chip;
    }

    [[nodiscard]] float dribbler() const
    {
        return m_dribbler;
    }

    [[nodiscard]] bool navigated() const
    {
        return m_navigated;
    }

    [[nodiscard]] bool halted() const
    {
        return m_halted;
    }

    [[nodiscard]] Trajectory2D currentTrajectory() const
    {
        return m_trajectory;
    }

    [[nodiscard]] Common::Vec2 currentMotion() const
    {
        if (Common::almostEqual(m_trajectory.getDuration(), 0.0f))
        {
            return {};
        }

        const float dt = 1.0f / Common::config().vision.vision_frame_rate;
        return m_trajectory.getVelocity(dt);
    }

    [[nodiscard]] Sender::Command currentCommand() const;

    Common::RobotState target;

    // TODO: these don't really belong here
    OneTouchDetector one_touch_detector;

    Common::Soccer::OneTouchType one_touch_type      = Common::Soccer::OneTouchType::OneTouch;
    bool                         one_touch_type_used = false;

private:
    const Common::RobotState *m_state = nullptr;

    ObstacleMap m_obs_map;
    Planner     m_planner;

    Trajectory2D m_trajectory;

    float m_shoot    = 0.0f;
    float m_chip     = 0.0f;
    float m_dribbler = 0.0f;

    bool m_navigated = false;
    bool m_halted    = false;

    void setObstacles(NavigationFlags t_flags = NavigationFlags::None);

    std::future<void> m_navigation_future;
    void navigateJob(Common::Vec2 t_dest, VelocityProfile t_profile, NavigationFlags t_flags);
};
} // namespace Tyr::Soccer
