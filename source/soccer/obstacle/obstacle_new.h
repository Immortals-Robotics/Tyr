#pragma once

namespace Tyr::Soccer
{
class BaseObstacle
{
public:
    virtual ~BaseObstacle() = default;

    virtual bool  IsInObstacle(float _x, float _y)    = 0;
    virtual float NearestDistance(float _x, float _y) = 0;
};

class CircleObstacle : public BaseObstacle
{
private:
    float x, y;
    float r;

public:
    CircleObstacle(float _x, float _y, float _r);
    bool  IsInObstacle(float _x, float _y);
    float NearestDistance(float _x, float _y);
};

class RectangleObstacle : public BaseObstacle
{
private:
    float x, y;
    float w, h;

public:
    RectangleObstacle(float _x, float _y, float _w, float _h);
    bool  IsInObstacle(float _x, float _y);
    float NearestDistance(float _x, float _y);
};

class HalfPlaneObstacle : public BaseObstacle
{
private:
    float w, h;

public:
    HalfPlaneObstacle(float _x, float _y, float _w, float _h);
    bool  IsInObstacle(float _x, float _y);
    float NearestDistance(float _x, float _y);
};

class ObsMap
{
private:
    std::vector<std::unique_ptr<BaseObstacle>> m_obstacles;

public:
    ObsMap();

    void AddCircle(float _x, float _y, float _r);
    void AddRectangle(float _x, float _y, float _w, float _h);

    bool  IsInObstacle(float _x, float _y);
    float NearestDistance(float _x, float _y);

    void resetMap();

    int getObsNum();
};
} // namespace Tyr::Soccer
