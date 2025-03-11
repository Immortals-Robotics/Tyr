#pragma once

#include "../kalman/state/robot.h"
#include "../kalman/system_model/robot.h"
#include "../kalman/measurement/position_model.h"
#include "../kalman/measurement/orientation_model.h"

namespace Tyr::Vision
{
class TrackedRobot
{
public:
    TrackedRobot()
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
        Kalman::Covariance<Filter::PositionMeasurement> positionMeasurementNoise{};
        positionMeasurementNoise.setZero();
        positionMeasurementNoise(Filter::PositionMeasurement::X,Filter::PositionMeasurement::X) = 1.0;    // x variance in mm² (1mm)²
        positionMeasurementNoise(Filter::PositionMeasurement::Y,Filter::PositionMeasurement::Y) = 1.0;    // y variance in mm² (1mm)²
        m_position_model.setCovariance(positionMeasurementNoise);

        Kalman::Covariance<Filter::OrientationMeasurement> orientationMeasurementNoise{};
        orientationMeasurementNoise.setZero();
        orientationMeasurementNoise(Filter::OrientationMeasurement::THETA,Filter::OrientationMeasurement::THETA) = 1.0;    // θ variance in degrees² (about 1°)²
        m_orientation_model.setCovariance(orientationMeasurementNoise);
    }

    // Initialize the position whenever it is lost and re-found. Use this for the first initial state too.
    void reset(const Common::Vec2 t_pos, const Common::Angle t_angle)
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
        robot_state.theta() = t_angle.deg();
        m_kalman.init(robot_state);
    }

    //  update the internal state using known vision data
    void update(const Common::Vec2 t_pos, const Common::Angle t_angle)
    {
        m_kalman.predict(m_system_model);

        Filter::PositionMeasurement pos_measurement{t_pos};
        m_kalman.update(m_position_model, pos_measurement);

        Filter::OrientationMeasurement orientation_measurement{t_angle};
        m_kalman.update(m_orientation_model, orientation_measurement);
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

    Common::Angle getAngle() const
    {
        const Filter::RobotState& state = m_kalman.getState();
        return Common::Angle::fromDeg(state.theta());
    }

    Common::Angle getAngularVelocity() const
    {
        // TODO:
        return {};
    }

private:
    Kalman::ExtendedKalmanFilter<Filter::RobotState> m_kalman;

    Filter::RobotSystemModel m_system_model;

    Filter::PositionMeasurementModel<Filter::RobotState> m_position_model;
    Filter::OrientationMeasurementModel m_orientation_model;
};
} // namespace Tyr::Vision
