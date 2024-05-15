#pragma once

namespace Tyr::Gui
{
class Renderer
{
public:
    Renderer();

    void beginDraw(const Common::FieldState &t_field);
    void endDraw();

    void draw(Common::Vec2 t_point, Common::Color t_color, bool t_is_filled, float t_thickness = 1);
    void draw(Common::Rect t_rect, Common::Color t_color, bool t_is_filled, float t_thickness = 1);
    void draw(Common::Circle t_circle, Common::Color t_color, bool t_is_filled, float t_thickness = 1);
    void drawCircleSector(Common::Circle t_circle, Common::Color t_color, float t_start_angle, float t_end_angle,
                          bool t_is_filled);
    void draw(Common::Line t_line, Common::Color t_color, float t_thickness = 1);
    void draw(Common::LineSegment t_line_segment, Common::Color t_color, float t_thickness = 1);
    void draw(Common::Triangle t_triangle, Common::Color t_color, bool t_is_filled = true, float t_thickness = 1);

    void draw(const Common::RawRobotState &t_robot);
    void draw(const Common::RawBallState &t_ball);
    void draw(const Common::RawWorldState &t_world);

    void draw(const Common::RobotState &t_robot);
    void draw(const Common::BallState &t_ball);
    void draw(const Common::WorldState &t_world);

    void draw(const Common::FieldState &t_field);

    void drawText(Common::Vec2 t_pos, const std::string &t_str, int t_font_size = 200,
                  Common::Color t_color = Common::Color::white());

    void draw(const Common::Debug::Wrapper &t_wrapper);

    Common::Vec2 getMousePosition();

private:
    static constexpr float kRobotArcAngle = 50.0f;
    static constexpr float kWindowBorder  = 8.0f;

    Camera2D m_camera;
    Font     m_font;

    Vector2 ConvertSignedVecToPixelVec(Common::Vec2 _signedVec);
};
} // namespace Tyr::Gui
