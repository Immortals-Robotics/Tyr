#pragma once

#include "ball_model.h"
#include "camera_calibration.h"

namespace Immortals::Common
{
struct FieldState
{
    float width  = 6000.0f;
    float height = 4500.0f;

    float goal_width  = 1800.0f;
    float goal_depth  = 180.0f;
    float goal_height = 200.0f;

    float boundary_width = 300.0f;

    float penalty_area_depth = 1800.0f;
    float penalty_area_width = 3600.0f;

    float center_circle_radius = 500.0f;

    float line_thickness = 10.0f;

    float goal_center_to_penalty_mark = 8000.0f;

    float ball_radius  = 21.5f;
    float robot_radius = 90.0f;

    std::array<CameraCalibration, Config::Vision::kCamCount> camera_calibrations;

    BallModelStraightTwoPhase ball_model_straight;
    BallModelChipFixedLoss    ball_model_chip;

    FieldState() = default;

    void update(const Protos::Ssl::Vision::GeometryData &t_data)
    {
        width  = t_data.field().field_length() / 2.0f;
        height = t_data.field().field_width() / 2.0f;

        goal_width = t_data.field().goal_width();
        goal_depth = t_data.field().goal_depth();
        if (t_data.field().has_goal_height())
            goal_height = t_data.field().goal_height();

        boundary_width = t_data.field().boundary_width();

        if (t_data.field().has_penalty_area_depth())
            penalty_area_depth = t_data.field().penalty_area_depth();
        if (t_data.field().has_penalty_area_width())
            penalty_area_width = t_data.field().penalty_area_width();

        if (t_data.field().has_center_circle_radius())
            center_circle_radius = t_data.field().center_circle_radius();

        if (t_data.field().has_line_thickness())
            line_thickness = t_data.field().line_thickness();

        if (t_data.field().has_goal_center_to_penalty_mark())
            goal_center_to_penalty_mark = t_data.field().goal_center_to_penalty_mark();

        if (t_data.field().has_ball_radius())
            ball_radius = t_data.field().ball_radius();

        if (t_data.field().has_max_robot_radius())
            robot_radius = t_data.field().max_robot_radius();

        for (const auto &calib : t_data.calib())
        {
            camera_calibrations[calib.camera_id()] = CameraCalibration{calib};
        }

        if (t_data.has_models())
        {
            if (t_data.models().has_straight_two_phase())
            {
                ball_model_straight = BallModelStraightTwoPhase{t_data.models().straight_two_phase()};
            }

            if (t_data.models().has_chip_fixed_loss())
            {
                ball_model_chip = BallModelChipFixedLoss{t_data.models().chip_fixed_loss()};
            }
        }
    }
};
} // namespace Immortals::Common
