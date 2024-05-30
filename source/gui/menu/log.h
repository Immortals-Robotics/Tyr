#pragma once
#include "config.h"

namespace Tyr::Gui
{
class LogMenu
{
public:
    void draw(const Common::Debug::Wrapper                                    &t_wrapper,
              const std::unordered_map<std::string, ConfigMenu::FilterNode *> &t_map);

private:
    int m_filter_level = 0;
};
} // namespace Tyr::Gui
