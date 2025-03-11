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
        state.setPosition(t_pos);
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

    // predicts the state in dt s in the future
    // this is similar to system-model's f function
    Filter::BallState predict(const float dt) const
    {
        // TODO: move these to the ball system model

        const Filter::BallState& state = m_kalman.getState();
        const Common::Vec2 position {state.x(), state.y()};
        const Common::Vec2 velocity {state.vx(), state.vy()};

        // can't predict stationary balls
        if (Common::almostEqual(velocity.length(), 0.0f))
        {
            return state;
        }

        // TODO: add acceleration to the state vector and let kalman compute it
        const float k  = 700.f; // ball deceleration (mm/s2)
        const Common::Vec2 deceleration = -velocity.normalized() * k;

        const float time_to_stop = velocity.length() / k;

        const float prediction_time = std::min(time_to_stop, dt);

        const Common::Vec2 predicted_velocity = velocity + deceleration * prediction_time;
        const Common::Vec2 displacement       = (velocity + predicted_velocity) * (0.5f * prediction_time);

        const Common::Vec2 predicted_position = position + displacement;

        Filter::BallState predicted_state{};
        predicted_state.setZero();
        predicted_state.x() = predicted_position.x;
        predicted_state.y() = predicted_position.y;
        predicted_state.vx() = predicted_velocity.x;
        predicted_state.vy() = predicted_velocity.y;

        return predicted_state;
    }

    const Filter::BallState& state() const
    {
        return m_kalman.getState();
    }

private:
    Kalman::ExtendedKalmanFilter<Filter::BallState> m_kalman;

    Filter::BallSystemModel m_system_model;

    Filter::PositionMeasurementModel<Filter::BallState> m_position_model;
};
} // namespace Tyr::Vision
