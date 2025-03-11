#pragma once

#include "../kalman/state/ball.h"
#include "../kalman/system_model/ball.h"
#include "../kalman/measurement/position_model.h"

namespace Tyr::Vision
{
class TrackedBall
{
public:
    TrackedBall()
    {
        const float dt = 1.f / Common::config().vision.vision_frame_rate;

        // process noise covariance
        Kalman::Covariance<Filter::BallState> processNoise{};
        processNoise.setZero();
        // Position variances (affected by velocity uncertainty)
        processNoise(Filter::BallState::X,Filter::BallState::X) = 100.0f * dt * dt * dt;  // x position variance (mm²)
        processNoise(Filter::BallState::Y,Filter::BallState::Y) = 100.0f * dt * dt * dt;  // y position variance (mm²)

        // Velocity variances (direct process noise)
        processNoise(Filter::BallState::VX,Filter::BallState::VX) = 2000.0f * dt;  // vx variance (mm²/s²)
        processNoise(Filter::BallState::VY,Filter::BallState::VY) = 2000.0f * dt;  // vy variance (mm²/s²)
        m_system_model.setCovariance(processNoise);

        // measurement noise covariance
        Kalman::Covariance<Filter::PositionMeasurement> measurementNoise{};
        measurementNoise.setZero();
        measurementNoise(Filter::PositionMeasurement::X,Filter::PositionMeasurement::X) = 4.0;    // x variance in mm²
        measurementNoise(Filter::PositionMeasurement::Y,Filter::PositionMeasurement::Y) = 4.0;    // y variance in mm²
        m_position_model.setCovariance(measurementNoise);

        reset({});
    }

    // Initialize the position whenever it is lost and re-found. Use this for the first initial state too.
    void reset(const Common::Vec2 t_pos)
    {
        // initial state error covariance
        Kalman::Covariance<Filter::BallState> initialStateCovariance{};
        initialStateCovariance.setZero();

        // Initial position uncertainty
        initialStateCovariance(Filter::BallState::X,Filter::BallState::X) = 100.0;  // x position variance (mm²)
        initialStateCovariance(Filter::BallState::Y,Filter::BallState::Y) = 100.0;  // y position variance (mm²)

        // Initial velocity uncertainty
        initialStateCovariance(Filter::BallState::VX,Filter::BallState::VX) = 1000.0;  // vx variance (mm²/s²)
        initialStateCovariance(Filter::BallState::VY,Filter::BallState::VY) = 1000.0;  // vy variance (mm²/s²)
        m_kalman.setCovariance(initialStateCovariance);

        Filter::BallState state{};
        state.setZero();
        state.x() = t_pos.x;
        state.y() = t_pos.y;
        m_kalman.init(state);
    }

    // propagated the state forward in time
    // this needs to be called every frame no matter if new data is available or not
    // if new vision data is available, it should be provided via update
    void predict()
    {
        m_kalman.predict(m_system_model);
    }

    //  update the internal state using known vision data
    void update(const Common::Vec2 t_pos)
    {
        Filter::PositionMeasurement pos_measurement{t_pos};
        m_kalman.update(m_position_model, pos_measurement);
    }

    Common::Vec2 getPosition() const
    {
        const Filter::BallState& state = m_kalman.getState();
        return {state.x(), state.y()};
    }

    Common::Vec2 getVelocity() const
    {
        const Filter::BallState& state = m_kalman.getState();
        return {state.vx(), state.vy()};
    }

private:
    Kalman::ExtendedKalmanFilter<Filter::BallState> m_kalman;

    Filter::BallSystemModel m_system_model;

    Filter::PositionMeasurementModel<Filter::BallState> m_position_model;
};
} // namespace Tyr::Vision
