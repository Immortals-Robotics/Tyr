#pragma once

namespace Immortals::Common
{
struct CameraCalibration
{
    unsigned camera_id = 0;

    float focal_length = 0.0f;

    Vec2 principal_point;

    float distortion = 0.0f;

    float q0 = 0.0f;
    float q1 = 0.0f;
    float q2 = 0.0f;
    float q3 = 0.0f;

    Vec3 t;

    Vec3 derived_camera_world_t;

    Vec2 pixel_image_size;

    CameraCalibration() = default;

    explicit CameraCalibration(const Protos::Ssl::Vision::GeometryCameraCalibration &t_calibration)
    {
        camera_id = t_calibration.camera_id();

        focal_length = t_calibration.focal_length();

        principal_point = Vec2{t_calibration.principal_point_x(), t_calibration.principal_point_y()};

        distortion = t_calibration.distortion();

        q0 = t_calibration.q0();
        q1 = t_calibration.q1();
        q2 = t_calibration.q2();
        q3 = t_calibration.q3();

        t = Vec3{t_calibration.tx(), t_calibration.ty(), t_calibration.tz()};

        derived_camera_world_t = Vec3{t_calibration.derived_camera_world_tx(), t_calibration.derived_camera_world_ty(),
                                      t_calibration.derived_camera_world_tz()};

        pixel_image_size = Vec2{static_cast<float>(t_calibration.pixel_image_width()),
                                static_cast<float>(t_calibration.pixel_image_height())};
    }
};
} // namespace Immortals::Common
