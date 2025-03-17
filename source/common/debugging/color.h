#pragma once

namespace Immortals::Common
{
struct Color
{
    Color() = default;

    constexpr Color(const float t_r, const float t_g, const float t_b, const float t_a = 1.0f)
        : r(t_r), g(t_g), b(t_b), a(t_a)
    {}

    explicit Color(const Protos::Immortals::Color &t_color) : Color(t_color.r(), t_color.g(), t_color.b(), t_color.a())
    {}

#if FEATURE_RAYLIB
    explicit Color(const ::Color &t_c) : Color(t_c.r / 255.0f, t_c.g / 255.0f, t_c.b / 255.0f, t_c.a / 255.0f)
    {}

    explicit operator ::Color() const
    {
        return {
            .r = static_cast<unsigned char>(r * 255.0f),
            .g = static_cast<unsigned char>(g * 255.0f),
            .b = static_cast<unsigned char>(b * 255.0f),
            .a = static_cast<unsigned char>(a * 255.0f),
        };
    }
#endif

#if FEATURE_IMGUI
    explicit operator ::ImVec4() const
    {
        return {r, g, b, a};
    }
#endif

    void fillProto(Protos::Immortals::Color *const t_color) const
    {
        t_color->set_r(r);
        t_color->set_g(g);
        t_color->set_b(b);
        t_color->set_a(a);
    }

    [[nodiscard]] constexpr Color transparent() const
    {
        return Color{r, g, b, a / 4.0f};
    }

    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 1.0f;

    // Common colors that match raylib ones
    static constexpr Color lightGray()
    {
        return {0.7843f, 0.7843f, 0.7843f, 1.0f};
    }

    static constexpr Color gray()
    {
        return {0.5098f, 0.5098f, 0.5098f, 1.0f};
    }

    static constexpr Color darkGray()
    {
        return {0.3137f, 0.3137f, 0.3137f, 1.0f};
    }

    static constexpr Color yellow()
    {
        return {0.9922f, 0.9765f, 0.0f, 1.0f};
    }

    static constexpr Color gold()
    {
        return {1.0f, 0.7961f, 0.0f, 1.0f};
    }

    static constexpr Color orange()
    {
        return {1.0f, 0.6314f, 0.0f, 1.0f};
    }

    static constexpr Color pink()
    {
        return {1.0f, 0.4275f, 0.7608f, 1.0f};
    }

    static constexpr Color red()
    {
        return {0.9020f, 0.1608f, 0.2157f, 1.0f};
    }

    static constexpr Color maroon()
    {
        return {0.7451f, 0.1294f, 0.2157f, 1.0f};
    }

    static constexpr Color green()
    {
        return {0.0f, 0.8941f, 0.1882f, 1.0f};
    }

    static constexpr Color lime()
    {
        return {0.0f, 0.6196f, 0.1843f, 1.0f};
    }

    static constexpr Color darkGreen()
    {
        return {0.0f, 0.4588f, 0.1725f, 1.0f};
    }

    static constexpr Color skyblue()
    {
        return {0.4000f, 0.7490f, 1.0f, 1.0f};
    }

    static constexpr Color blue()
    {
        return {0.0f, 0.4745f, 0.9451f, 1.0f};
    }

    static constexpr Color darkBlue()
    {
        return {0.0f, 0.3216f, 0.6745f, 1.0f};
    }

    static constexpr Color purple()
    {
        return {0.7843f, 0.4784f, 1.0f, 1.0f};
    }

    static constexpr Color violet()
    {
        return {0.5294f, 0.2353f, 0.7451f, 1.0f};
    }

    static constexpr Color darkPurple()
    {
        return {0.4392f, 0.1216f, 0.4941f, 1.0f};
    }

    static constexpr Color beige()
    {
        return {0.8275f, 0.6902f, 0.5137f, 1.0f};
    }

    static constexpr Color brown()
    {
        return {0.4980f, 0.4157f, 0.3098f, 1.0f};
    }

    static constexpr Color darkBrown()
    {
        return {0.2980f, 0.2471f, 0.1843f, 1.0f};
    }

    static constexpr Color white()
    {
        return {1.0f, 1.0f, 1.0f, 1.0f};
    }

    static constexpr Color black()
    {
        return {0.0f, 0.0f, 0.0f, 1.0f};
    }

    static constexpr Color blank()
    {
        return {0.0f, 0.0f, 0.0f, 0.0f};
    }

    static constexpr Color magenta()
    {
        return {1.0f, 0.0f, 1.0f, 1.0f};
    }

    static Color lerp(const Color &t_c1, const Color &t_c2, const float t_amount)
    {
        const float clamped_amount = std::clamp(t_amount, 0.0f, 1.0f);
        return {t_c1.r * (1.0f - clamped_amount) + t_c2.r * clamped_amount,
                t_c1.g * (1.0f - clamped_amount) + t_c2.g * clamped_amount,
                t_c1.b * (1.0f - clamped_amount) + t_c2.b * clamped_amount,
                t_c1.a * (1.0f - clamped_amount) + t_c2.a * clamped_amount};
    }
};
} // namespace Immortals::Common
