#pragma once
#include "filter.h"

namespace Tyr::Gui
{
class LogMenu
{
public:
    void draw(const Common::Debug::Wrapper &t_wrapper, const FilterMenu::Map &t_map);

private:
    int m_filter_level = 0;
};
} // namespace Tyr::Gui
