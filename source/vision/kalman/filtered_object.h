#pragma once

#include "state/robot.h"
#include "measurement/position_model.h"
#include "system_model/robot.h"

namespace Tyr::Vision
{
class FilteredObject
{
public:
    FilteredObject()
    {
        const float dt = 1.f / Common::config().vision.vision_frame_rate;

        // process noise covariance
        Kalman::Covariance<Filter::RobotState> processNoise{};
        processNoise.setZero();
        // Position variances (affected by velocity uncertainty)
        processNoise(Filter::RobotState::X,Filter::RobotState::X) = 100.0f * dt * dt * dt;  // x position variance (mm²)
        processNoise(Filter::RobotState::Y,Filter::RobotState::Y) = 100.0f * dt * dt * dt;  // y position variance (mm²)

        // Velocity variances (direct process noise)
        processNoise(Filter::RobotState::VX,Filter::RobotState::VX) = 2000.0f * dt;  // vx variance (mm²/s²)
        processNoise(Filter::RobotState::VY,Filter::RobotState::VY) = 2000.0f * dt;  // vy variance (mm²/s²)

        // Orientation variance
        processNoise(Filter::RobotState::THETA,Filter::RobotState::THETA) = 100.0f * dt;   // θ variance (degrees²)
        m_system_model.setCovariance(processNoise);

        // measurement noise covariance
        Kalman::Covariance<Filter::PositionMeasurement> measurementNoise{};
        measurementNoise.setZero();
        measurementNoise(Filter::PositionMeasurement::X,Filter::PositionMeasurement::X) = 1.0;    // x variance in mm² (1mm)²
        measurementNoise(Filter::PositionMeasurement::Y,Filter::PositionMeasurement::Y) = 1.0;    // y variance in mm² (1mm)²
        m_position_model.setCovariance(measurementNoise);
    }

    // Initialize the position whenever it is lost and re-found. Use this for the first initial state too.
    void initializePos(const Common::Vec2 t_pos)
    {
        // initial state error covariance
        Kalman::Covariance<Filter::RobotState> initialStateCovariance{};
        initialStateCovariance.setZero();

        // Initial position uncertainty
        initialStateCovariance(Filter::RobotState::X,Filter::RobotState::X) = 100.0;  // x position variance (mm²)
        initialStateCovariance(Filter::RobotState::Y,Filter::RobotState::Y) = 100.0;  // y position variance (mm²)

        // Initial velocity uncertainty
        initialStateCovariance(Filter::RobotState::VX,Filter::RobotState::VX) = 1000.0;  // vx variance (mm²/s²)
        initialStateCovariance(Filter::RobotState::VY,Filter::RobotState::VY) = 1000.0;  // vy variance (mm²/s²)

        // Initial orientation uncertainty
        initialStateCovariance(Filter::RobotState::THETA,Filter::RobotState::THETA) = 25.0;   // θ variance (degrees²)

        m_kalman.setCovariance(initialStateCovariance);

        Filter::RobotState robot_state{};
        robot_state.x() = t_pos.x;
        robot_state.y() = t_pos.y;
        m_kalman.init(robot_state);

        m_initialized = true;
    }

    //  update the internal state using known vision data
    void updatePosition(const Common::Vec2 t_pos)
    {
        if (!m_initialized)
            initializePos(t_pos);

        m_kalman.predict(m_system_model);

        Filter::PositionMeasurement measurement{t_pos};
        m_kalman.update(m_position_model, measurement);
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
};
} // namespace Tyr::Vision
