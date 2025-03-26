#pragma once

namespace Tyr::Soccer
{
// Concept defining required interface for shape types
template <typename Shape>
concept ShapeConcept = requires(const Shape &shape, const Common::Vec2 point, const float t_margin)
{
    { shape.inside(point, t_margin) } -> std::same_as<bool>;
    { shape.distance(point) } -> std::same_as<float>;
};

enum class Physicality
{
    None = 0,

    Physical = BIT(0),
    Virtual = BIT(1),

    All = Physical | Virtual,
};

ENABLE_ENUM_FLAG_OPERATORS(Physicality);

template <ShapeConcept Shape>
class Obstacle
{
public:
    Obstacle(const Shape &t_shape, const Physicality t_physicality) : m_shape(t_shape), m_physicality(t_physicality)
    {}

    bool inside(const Common::Vec2 t_point, const float t_margin = 0.0f) const
    {
        return m_shape.inside(t_point, t_margin);
    }

    float distance(const Common::Vec2 t_point) const
    {
        return m_shape.distance(t_point);
    }

    Physicality physicality() const
    {
        return m_physicality;
    }

protected:
    Shape       m_shape;
    Physicality m_physicality = Physicality::None;
};

using RectObstacle   = Obstacle<Common::Rect>;
using CircleObstacle = Obstacle<Common::Circle>;
} // namespace Tyr::Soccer
