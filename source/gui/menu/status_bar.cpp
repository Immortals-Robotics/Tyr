#include "status_bar.h"

namespace Tyr::Gui
{
void StatusBar::draw(const Common::Debug::Wrapper &t_wrapper)
{
    for (const auto &[name, time] : t_wrapper.execution_times)
    {
        auto &interval_filter = m_interval_filters[name];
        auto &duration_filter = m_duration_filters[name];
        interval_filter.add(time.interval);
        duration_filter.add(time.duration);

        ImGui::Bullet();

        ImGui::Text("%s [ fps: %.2f, exec: %.2f ]", name.c_str(), 1.0f / interval_filter.current().seconds(),
                    duration_filter.current().seconds() * 1000.0f);

        ImGui::SameLine();
    }
}
} // namespace Tyr::Gui
