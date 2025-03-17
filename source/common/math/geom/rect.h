#pragma once

namespace Immortals::Common
{
struct Rect
{
    Rect() = default;

    Rect(const Vec2 t_p1, const Vec2 t_p2)
        : min(std::min(t_p1.x, t_p2.x), std::min(t_p1.y, t_p2.y)),
          max(std::max(t_p1.x, t_p2.x), std::max(t_p1.y, t_p2.y))
    {}

    Rect(const Vec2 t_p, const float t_w, const float t_h) : Rect(t_p, t_p + Vec2(t_w, t_h))
    {}

    explicit Rect(const Protos::Immortals::Rect &t_rect) : Rect(Vec2{t_rect.min()}, Vec2{t_rect.max()})
    {}

#if FEATURE_RAYLIB
    explicit Rect(const ::Rectangle &t_rect) : Rect(Vec2{t_rect.x, t_rect.y}, t_rect.width, t_rect.height)
    {}

    explicit operator ::Rectangle() const
    {
        return {.x = min.x, .y = min.y, .width = width(), .height = height()};
    }
#endif

    void fillProto(Protos::Immortals::Rect *const t_rect) const
    {
        min.fillProto(t_rect->mutable_min());
        max.fillProto(t_rect->mutable_max());
    }

    bool inside(const Vec2 t_point, const float margin = 0.f) const
    {
        return distance(t_point) <= margin;
    }

    float distance(const Vec2 t_point) const
    {
        // Inside distance (negative if inside)
        const float d = std::max({
            min.x - t_point.x,
            t_point.x - max.x,
            min.y - t_point.y,
            t_point.y - max.y});

        return d;
    }

    bool intersects(const Rect &t_rect) const
    {
        return (min.x <= t_rect.max.x && max.x >= t_rect.min.x && min.y <= t_rect.max.y && max.y >= t_rect.min.y);
    }

    std::vector<Vec2> intersection(const Line &t_line) const
    {
        int  n_sol = 0;
        std::vector<Vec2> sols;

        std::vector<LineSegment> segments;
        segments.push_back({min, Vec2(min.x, max.y)});
        segments.push_back({min, Vec2(max.x, min.y)});
        segments.push_back({max, Vec2(min.x, max.y)});
        segments.push_back({max, Vec2(max.x, min.y)});

        for (const auto &segment : segments)
        {
            auto intersect = t_line.intersect(segment);
            if(intersect.has_value()) {
                sols.push_back(intersect.value());
            }
        }

        return sols;
    }

    Vec2 nearestOutside(const Vec2 t_point) const
    {
        constexpr float kExtension = 5.0f;

        const float delta_x_min = abs(t_point.x - min.x);
        const float delta_x_max = abs(t_point.x - max.x);
        const float delta_y_min = abs(t_point.y - min.y);
        const float delta_y_max = abs(t_point.y - max.y);

        const float min_xy = std::min(std::min(delta_x_min, delta_x_max), std::min(delta_y_min, delta_y_max));

        if (delta_x_min == min_xy)
        {
            return Vec2(min.x - kExtension, t_point.y);
        }
        else if (delta_x_max == min_xy)
        {
            return Vec2(max.x + kExtension, t_point.y);
        }
        else if (delta_y_min == min_xy)
        {
            return Vec2(t_point.x, min.y - kExtension);
        }
        else
        {
            //(deltaYmax == min_xy)
            return Vec2(t_point.x, max.y + kExtension);
        }
    }

    bool isCircleCross(const Vec2 t_point) const
    {
        return (min.x <= t_point.x) && (max.x >= t_point.x) && (min.y <= t_point.y) && (max.y >= t_point.y);
    }

    Vec2 size() const
    {
        return max - min;
    }

    float width() const
    {
        return max.x - min.x;
    }

    float height() const
    {
        return max.y - min.y;
    }

    Vec2 center() const
    {
        return (min + max) / 2.0f;
    }

    Vec2 min;
    Vec2 max;
};
} // namespace Immortals::Common
