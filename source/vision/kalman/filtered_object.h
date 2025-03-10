#pragma once

#include "state/robot.h"
#include "measurement/position_model.h"
#include "system_model/robot.h"

namespace Tyr::Vision
{
class FilteredObject
{
public:
    FilteredObject() : m_kalman(1)
    {}

    // Initialize the position whenever it is lost and re-found. Use this for the first initial state too.
    void initializePos(Common::Vec2 t_pos)
    {
        const Filter::RobotState robot_state{t_pos, {}};
        m_kalman.init(robot_state);

        m_initialized = true;
    }

    //  update the internal state using known vision data
    void updatePosition(const Common::Vec2 t_pos)
    {
        if (!m_initialized)
            initializePos(t_pos);

        m_kalman.predict(m_system_model);

        const float dt = 1.f / Common::config().vision.vision_frame_rate;
        Common::Vec2 raw_velocity = (t_pos - m_last_raw_pos) / dt;
        Filter::PositionMeasurement measurement{t_pos, raw_velocity};
        m_kalman.update(m_position_model, measurement);

        m_last_raw_pos = t_pos;
    }

    Common::Vec2 getPosition() const
    {
        const Filter::RobotState& state = m_kalman.getState();
        return {state.x(), state.y()};
    }

    Common::Vec2 getVelocity() const
    {
        const Filter::RobotState& state = m_kalman.getState();
        return {state.vx(), state.vy()};
    }

private:
    bool m_initialized = false;

    Kalman::SquareRootUnscentedKalmanFilter<Filter::RobotState> m_kalman;
    Filter::RobotSystemModel m_system_model;
    Filter::PositionMeasurementModel m_position_model;

    Common::Vec2 m_last_raw_pos;
};
} // namespace Tyr::Vision
