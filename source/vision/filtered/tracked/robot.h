#pragma once

#include "../../kalman/state/robot.h"
#include "../../kalman/system_model/robot.h"
#include "../../kalman/measurement/position_model.h"
#include "../../kalman/measurement/orientation_model.h"

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
        processNoise(Filter::RobotState::THETA_COS,Filter::RobotState::THETA_COS) = 0.03f * dt;   // θ variance (sin(degrees)²)
        processNoise(Filter::RobotState::THETA_SIN,Filter::RobotState::THETA_SIN) = 0.03f * dt;   // θ variance (sin(degrees)²)

        // Orientation velocity variance
        processNoise(Filter::RobotState::OMEGA,Filter::RobotState::OMEGA) = 0.3f * dt;   // θ variance ((rad/s)²)

        m_system_model.setCovariance(processNoise);

        // measurement noise covariance
        Kalman::Covariance<Filter::PositionMeasurement> positionMeasurementNoise{};
        positionMeasurementNoise.setZero();
        positionMeasurementNoise(Filter::PositionMeasurement::X,Filter::PositionMeasurement::X) = 4.0;    // x variance in mm²
        positionMeasurementNoise(Filter::PositionMeasurement::Y,Filter::PositionMeasurement::Y) = 4.0;    // y variance in mm²
        m_position_model.setCovariance(positionMeasurementNoise);

        Kalman::Covariance<Filter::OrientationMeasurement> orientationMeasurementNoise{};
        orientationMeasurementNoise.setZero();
        orientationMeasurementNoise(Filter::OrientationMeasurement::THETA_COS,Filter::OrientationMeasurement::THETA_COS) = 0.0003f;    // θ variance in sin(rad)² (about 1°)²
        orientationMeasurementNoise(Filter::OrientationMeasurement::THETA_SIN,Filter::OrientationMeasurement::THETA_SIN) = 0.0003f;    // θ variance in sin(rad)² (about 1°)²
        m_orientation_model.setCovariance(orientationMeasurementNoise);

        reset({},{});
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
        initialStateCovariance(Filter::RobotState::THETA_COS,Filter::RobotState::THETA_COS) = 0.01f;   // θ variance (sin(degrees)²)
        initialStateCovariance(Filter::RobotState::THETA_SIN,Filter::RobotState::THETA_SIN) = 0.01f;   // θ variance (sin(degrees)²)

        // Initial orientation velocity uncertainty
        initialStateCovariance(Filter::RobotState::OMEGA,Filter::RobotState::OMEGA) = 0.1f;   // θ variance (sin(degrees)²)

        m_kalman.setCovariance(initialStateCovariance);

        Filter::RobotState robot_state{};
        robot_state.setZero();
        robot_state.setPosition(t_pos);
        robot_state.setAngle(t_angle);
        m_kalman.init(robot_state);

        m_confidence = 0;
    }

    // propagated the state forward in time
    // this needs to be called every frame no matter if new data is available or not
    // if new vision data is available, it should be provided via update
    void predict()
    {
        m_kalman.predict(m_system_model);

        m_confidence = std::clamp(m_confidence - 1, 0, 100);
    }

    //  update the internal state using known vision data
    void update(const Common::Vec2 t_pos, const Common::Angle t_angle)
    {
        Filter::PositionMeasurement pos_measurement{t_pos};
        m_kalman.update(m_position_model, pos_measurement);

        Filter::OrientationMeasurement orientation_measurement{t_angle};
        m_kalman.update(m_orientation_model, orientation_measurement);

        m_confidence = std::clamp(m_confidence + 2, 0, 100);
    }

    Filter::RobotState predictOwn(const float dt, const std::deque<Sender::Command>& history) const
    {
        Common::Vec2 position = state().position();

        const float vision_dt = 1.0f/ Common::config().vision.vision_frame_rate;
        const int history_use_count = std::floor(dt / vision_dt);

        // not enough commands in the history
        if (history_use_count == 0 || history.size() < history_use_count)
        {
            position += state().velocity() * dt;
        }
        else
        {
            for (int i = 0; i <history_use_count; i++)
            {
                const Sender::Command& cmd = *(history.end() - (i + 1));
                position += cmd.motion * vision_dt;
            }
        }

        Filter::RobotState predicted_state = state();
        predicted_state.setPosition(position);
        if (!history.empty())
            predicted_state.setVelocity(history.back().motion);
        return predicted_state;
    }

    Filter::RobotState predictOpp(const float dt) const
    {
        Common::Vec2 position = state().position();
        Common::Angle angle = state().angle();

        position += state().velocity() * dt;
        angle += state().angularVelocity() * dt;

        Filter::RobotState predicted_state = state();
        predicted_state.setPosition(position);
        predicted_state.setAngle(angle);
        return predicted_state;
    }

    const Filter::RobotState& state() const
    {
        return m_kalman.getState();
    }

    int confidence() const
    {
        return m_confidence;
    }

    void setConfidence(const int t_confidence)
    {
        m_confidence = t_confidence;
    }

private:
    Kalman::ExtendedKalmanFilter<Filter::RobotState> m_kalman;

    Filter::RobotSystemModel m_system_model;

    Filter::PositionMeasurementModel<Filter::RobotState> m_position_model;
    Filter::OrientationMeasurementModel m_orientation_model;

    // 0: never seen it, what's that?
    // 100: seen just now
    int m_confidence = 0;
};
} // namespace Tyr::Vision
