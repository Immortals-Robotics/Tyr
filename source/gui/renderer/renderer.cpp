#include "renderer.h"

namespace Tyr::Gui
{
Renderer::Renderer()
{
    const std::filesystem::path data_dir(DATA_DIR);
    const std::filesystem::path font_path = data_dir / "fonts/open-sans-regular.ttf";

    m_font = LoadFont(font_path.string().c_str());
}

void Renderer::begin(const Common::FieldState &t_field, const Common::Rect window_rect)
{
    const Common::Vec2 offset = window_rect.min + window_rect.size() / 2.0f;

    const Common::Vec2 field_size = {t_field.width * 2.0f + 4.0f * t_field.boundary_width,
                                     t_field.height * 2.0f + 4.0f * t_field.boundary_width};

    const Common::Vec2 ratio = window_rect.size() / field_size;

    const float zoom = window_rect.size().x < window_rect.size().y ? (ratio.x > ratio.y ? ratio.x : ratio.y)
                                                                   : (ratio.x > ratio.y ? ratio.y : ratio.x);

    m_camera = Camera2D{
        .offset   = static_cast<Vector2>(offset), // Offset from the target
        .target   = Vector2{0.0f, 0.0f},          // Camera target (center of the screen)
        .rotation = 0.0f,                         // Camera rotation in degrees
        .zoom     = zoom,                         // Camera zoom (1.0f means no zoom)
    };

    BeginMode2D(m_camera);
    BeginScissorMode(window_rect.min.x, window_rect.min.y, window_rect.size().x, window_rect.size().y);
}

void Renderer::end()
{
    EndScissorMode();
    EndMode2D();
}

void Renderer::draw(const Common::RawWorldState &t_world)
{
    for (const auto &ball : t_world.balls)
        draw(ball);
    for (const auto &robot : t_world.yellow_robots)
        draw(robot);
    for (const auto &robot : t_world.blue_robots)
        draw(robot);
}

void Renderer::draw(const Common::WorldState &t_world)
{
    draw(t_world.ball);
    for (const auto &robot : t_world.own_robot)
        draw(robot);
    for (const auto &robot : t_world.opp_robot)
        draw(robot);
}
} // namespace Tyr::Gui
