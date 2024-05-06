#pragma once

namespace Tyr::Gui
{
class Renderer
{
public:
    Renderer(Common::Vec2 _wSize, float _upScalingFactor);
    void init();

    void draw(Common::Vec2 t_point, Common::Color t_color, bool t_is_filled, float t_thickness = 1);
    void draw(Common::Rect t_rect, Common::Color t_color, bool t_is_filled, float t_thickness = 1);
    void draw(Common::Circle t_circle, Common::Color t_color, bool t_is_filled, float t_thickness = 1);
    void drawCircleSector(Common::Circle t_circle, Common::Color t_color, Common::Angle t_start_angle,
                          Common::Angle t_end_angle, bool t_is_filled);
    void draw(Common::Line t_line, Common::Color t_color, float t_thickness = 1);
    void draw(Common::LineSegment t_line_segment, Common::Color t_color, float t_thickness = 1);
    void draw(Common::Triangle t_triangle, Common::Color t_color, bool t_is_filled = true, float t_thickness = 1);
    void drawText(Common::Vec2 t_pos, const std::string &t_str, int t_font_size = 12,
                  Common::Color t_color = Common::Color::white());

    void drawField(const Protos::SSL_GeometryFieldSize &data);
    void drawRobots(const google::protobuf::RepeatedPtrField<Protos::SSL_DetectionRobot> &data,
                    Common::TeamColor                                                     color);
    void drawBalls(const google::protobuf::RepeatedPtrField<Protos::SSL_DetectionBall> &data, bool draw_goal_lines);

    void drawRobot(const Protos::SSL_DetectionRobot &robot, Common::TeamColor color);
    void drawBall(const Protos::SSL_DetectionBall &ball, bool draw_goal_lines);

    void drawShapesUdp(const google::protobuf::RepeatedPtrField<Protos::Immortals::Debug::Draw> &draws);

    void applyShader();

    Common::Vec2  getMousePosition();
    RenderTexture visualizationTexture, shaderVisualizationTexture;

private:
    const float         ballRadius;
    const float         robotRadius;
    const Common::Angle robotArcAngle;
    const float         m_window_border;
    Common::Vec2        m_w_size;
    Common::Vec2        overallFieldSize;
    float               m_zoom_scale;
    float               m_upscaling_factor;
    Common::Vec2        m_avil_size;
    Font                visualizationFont;
    Common::Vec2        m_mouse_pos;

    Shader fxaaShader;

    void    CalculateZoom();
    Vector2 ConvertSignedVecToPixelVec(Common::Vec2 _signedVec);
    int     ConvertRealityUnitToPixels(float _value);
    void    sortPointsClockwise(Common::Vec2 &_p1, Common::Vec2 &_p2, Common::Vec2 &_p3);
    void    calculateMousePos();
};
} // namespace Tyr::Gui
