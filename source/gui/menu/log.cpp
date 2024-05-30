#include "log.h"

namespace Tyr::Gui
{
void LogMenu::draw(const Common::Debug::Wrapper                                    &t_wrapper,
                   const std::unordered_map<std::string, FilterMenu::FilterNode *> &t_map)
{
    const char *filter_choices[] = {"Trace - 0", "Debug - 1", "Info - 2", "Warning - 3", "Error - 4", "Critical - 5"};

    ImGui::Combo("Log Level Filter", &m_filter_level, filter_choices, IM_ARRAYSIZE(filter_choices));
    ImGui::Separator();
    if (ImGui::BeginChild("Scrolling", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false,
                          ImGuiWindowFlags_HorizontalScrollbar))
    {
        for (const auto &line : t_wrapper.logs)
        {
            if (static_cast<int>(line.level) >= m_filter_level && FilterMenu::applyFilter(line.source, t_map))
            {
                const std::filesystem::path file_path{line.source.file};

                const std::string text =
                    fmt::format("[{:%a %H:%M:%S}] [{}] [{}:{}] {}", t_wrapper.time, line.levelName(),
                                file_path.filename(), line.source.line, line.text);

                ImGui::TextColored(static_cast<ImVec4>(line.color()), "%s", text.c_str());
            }
        }
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
}
} // namespace Tyr::Gui
