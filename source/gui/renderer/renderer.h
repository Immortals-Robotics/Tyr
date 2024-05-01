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
    void drawLineSegment(Common::LineSegment line_segment, Color _color, float _thickness = 1, unsigned char _transparency = 255);
    void drawText(Common::Vec2 _pos, std::string _str, int _fontSize = 12, Color _color = WHITE,
                  unsigned char _transparency = 255);

    void drawField(const Protos::SSL_GeometryFieldSize &data);
    void drawRobots(const google::protobuf::RepeatedPtrField<Protos::SSL_DetectionRobot> &data,
                    Common::TeamColor                                                     color);
    void drawBalls(const google::protobuf::RepeatedPtrField<Protos::SSL_DetectionBall> &data);

    void drawRobot(const Protos::SSL_DetectionRobot &robot, Common::TeamColor color);
    void drawBall(const Protos::SSL_DetectionBall &ball);

    void applyShader();

    RenderTexture visualizaionTexture, shaderVisualizationTexture;

private:
    const float  ballRadius;
    const float  robotRadius;
    const float  robotArcAngle;
    Common::Vec2 wSize;
    Common::Vec2 overallFieldSize;
    float        zoomScale;
    float        upScalingFactor;
    Font         visualizationFont;

    Shader fxaaShader;

    void    CalculateZoom();
    Vector2 ConvertSignedVecToPixelVec(Common::Vec2 _signedVec);
    int     ConvertRealityUnitToPixels(float _value);
};
} // namespace Tyr::Gui
