#include "status_bar.h"

namespace Tyr::Gui
{
void StatusBar::draw(const Common::Debug::Wrapper &t_wrapper)
{
    static std::unordered_map<std::string, Common::MedianFilter<Common::Duration, 30>> interval_filters;
    static std::unordered_map<std::string, Common::MedianFilter<Common::Duration, 30>> duration_filters;
    for (const auto &[name, time] : t_wrapper.execution_times)
    {
        ImGui::Spacing();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3);

        auto &interval_filter = interval_filters[name];
        auto &duration_filter = duration_filters[name];
        interval_filter.add(time.interval);
        duration_filter.add(time.duration);

        ImGui::Text("%s fps: %.2f, exec: %.2f", name.c_str(), 1.0f / interval_filter.current().seconds(),
                    duration_filter.current().seconds() * 1000.0f);
    }
}
} // namespace Tyr::Gui
