#pragma once
#include "config_menu.h"

namespace Tyr::Gui
{
class LogMenu
{
private:
    int m_filter_level = 0;
    ImVec2 m_window_size;
    ImVec2 m_window_pos;
    bool m_is_right = false;

    void drawTerminal(const Common::Debug::Wrapper                                    &t_wrapper,
                      const std::unordered_map<std::string, ConfigMenu::FilterNode *> &t_map);

public:
    void draw(const Common::Debug::Wrapper                                    &t_wrapper,
              const std::unordered_map<std::string, ConfigMenu::FilterNode *> &t_map);
    LogMenu()  = default;
    ~LogMenu() = default;
};
} // namespace Tyr::Gui