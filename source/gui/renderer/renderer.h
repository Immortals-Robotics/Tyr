#pragma once

namespace Tyr::Gui
{
class Renderer
{
public:
    Renderer(Common::Vec2 _wSize, float _upScalingFactor);
    void init();

    void drawRect(Common::Rect rect, Color _color, bool _isFilled, float _thickness = 1,
                  unsigned char transparency = 255);
    void drawCircle(Common::Circle circle, Color _color, bool _isFilled, float _thickness = 1,
                    unsigned char _transparency = 255);
    void drawCircleSector(Common::Circle circle, Color _color, float _startAngle, float _endAngle, bool _isFilled,
                          unsigned char _transparency = 255);
    void drawLineSegment(Common::LineSegment line_segment, Color _color, float _thickness = 1,
                         unsigned char _transparency = 255);
    void drawText(Common::Vec2 _pos, std::string _str, int _fontSize = 12, Color _color = WHITE,
                  unsigned char _transparency = 255);
    void drawTriangle(Common::Vec2 _p1, Common::Vec2 _p2, Common::Vec2 _p3, Color _color, bool _isFilled = true,
                      unsigned char _transparency = 255);

    void drawField(const Protos::SSL_GeometryFieldSize &data);
    void drawRobots(const google::protobuf::RepeatedPtrField<Protos::SSL_DetectionRobot> &data,
                    Common::TeamColor                                                     color);
    void drawBalls(const google::protobuf::RepeatedPtrField<Protos::SSL_DetectionBall> &data, bool draw_goal_lines);

    void drawRobot(const Protos::SSL_DetectionRobot &robot, Common::TeamColor color);
    void drawBall(const Protos::SSL_DetectionBall &ball, bool draw_goal_lines);

    void drawCirclesUdp(const google::protobuf::RepeatedPtrField<Protos::Immortals::Debug_Circle> &circles);
    void drawRectsUdp(const google::protobuf::RepeatedPtrField<Protos::Immortals::Debug_Rect> &rects);
    void drawPointsUdp(const google::protobuf::RepeatedPtrField<Protos::Immortals::Debug_Point> &points);
    void drawLinesUdp(const google::protobuf::RepeatedPtrField<Protos::Immortals::Debug_Line> &lines);

    void applyShader();

    Common::Vec2  getMousePosition();
    RenderTexture visualizaionTexture, shaderVisualizationTexture;

private:
    const float  ballRadius;
    const float  robotRadius;
    const float  robotArcAngle;
    const float  m_window_border;
    Common::Vec2 m_w_size;
    Common::Vec2 overallFieldSize;
    float        m_zoom_scale;
    float        m_upscaling_factor;
    Common::Vec2 m_avil_size;
    Font         visualizationFont;
    Common::Vec2 m_mouse_pos;

    Shader fxaaShader;

    void    CalculateZoom();
    Vector2 ConvertSignedVecToPixelVec(Common::Vec2 _signedVec);
    int     ConvertRealityUnitToPixels(float _value);
    void    sortPointsClockwise(Common::Vec2 &_p1, Common::Vec2 &_p2, Common::Vec2 &_p3);
    void    calculateMousePos();
};
} // namespace Tyr::Gui
