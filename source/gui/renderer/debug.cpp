#include "renderer.h"

namespace Tyr::Gui
{
void Renderer::draw(const Common::Debug::Wrapper                                    &t_wrapper,
                    const std::unordered_map<std::string, ConfigMenu::FilterNode *> &t_map)
{
    for (const auto &draw : t_wrapper.draws)
    {
        if (ConfigMenu::applyFilter(draw.source, t_map))
        {
            if (auto point = std::get_if<Common::Vec2>(&draw.shape); point)
                this->draw(*point, draw.color, draw.thickness);
            else if (auto line = std::get_if<Common::Line>(&draw.shape); line)
                this->draw(*line, draw.color, draw.thickness);
            else if (auto segment = std::get_if<Common::LineSegment>(&draw.shape); segment)
                this->draw(*segment, draw.color, draw.thickness);
            else if (auto rect = std::get_if<Common::Rect>(&draw.shape); rect)
                this->draw(*rect, draw.color, draw.filled, draw.thickness);
            else if (auto circle = std::get_if<Common::Circle>(&draw.shape); circle)
                this->draw(*circle, draw.color, draw.filled, draw.thickness);
            else if (auto triangle = std::get_if<Common::Triangle>(&draw.shape); triangle)
                this->draw(*triangle, draw.color, draw.filled, draw.thickness);
            else
                Common::logWarning("Unsupported draw.shape type: {}", draw.shape.index());
        }
    }
}

} // namespace Tyr::Gui
