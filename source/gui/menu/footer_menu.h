#pragma once
#include "config_menu.h"

namespace Tyr::Gui
{
class FooterMenu
{
private:
    int m_filter_level = 0;
    bool m_is_plot_maximized = false;
    std::string m_plot_resize_icon = "\uf31e";

    void drawTerminal(const Common::Debug::Wrapper                                    &t_wrapper,
                      const std::unordered_map<std::string, ConfigMenu::FilterNode *> &t_map);

    void drawPlot();
public:
    void draw(const Common::Debug::Wrapper                                    &t_wrapper,
              const std::unordered_map<std::string, ConfigMenu::FilterNode *> &t_map);
    FooterMenu()  = default;
    ~FooterMenu() = default;
};
} // namespace Tyr::Gui