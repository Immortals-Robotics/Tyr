#pragma once

namespace Tyr::Soccer
{
class BaseObstacle
{
public:
    virtual ~BaseObstacle() = default;

    virtual bool  IsInObstacle(Common::Vec2 t_point)    = 0;
    virtual float NearestDistance(Common::Vec2 t_point) = 0;
};

class CircleObstacle final : public BaseObstacle
{
private:
    Common::Circle m_circle;

public:
    CircleObstacle(const Common::Circle t_circle) : m_circle(t_circle)
    {}

    bool  IsInObstacle(Common::Vec2 t_point) override;
    float NearestDistance(Common::Vec2 t_point) override;
};

class RectangleObstacle final : public BaseObstacle
{
private:
    Common::Rect m_rect;

public:
    RectangleObstacle(const Common::Rect t_rect) : m_rect(t_rect)
    {}

    bool  IsInObstacle(Common::Vec2 t_point) override;
    float NearestDistance(Common::Vec2 t_point) override;
};

class ObsMap
{
private:
    std::vector<std::unique_ptr<BaseObstacle>> m_obstacles;

public:
    ObsMap();

    void addCircle(Common::Circle t_circle);
    void addRectangle(Common::Rect t_rect);

    bool  isInObstacle(Common::Vec2 t_point);
    float nearestDistance(Common::Vec2 t_point);

	bool collisionDetect(const Common::Vec2 p1, const Common::Vec2 p2);

    void resetMap();

    int getObsNum();
};

extern ObsMap obs_map;

} // namespace Tyr::Soccer
