#include "log_menu.h"

namespace Tyr::Gui
{
void LogMenu::drawTerminal(const Common::Debug::Wrapper &t_wrapper)
{
    const char *filter_choices[] = {"Trace - 0", "Debug - 1", "Info - 2", "Warning - 3", "Error - 4", "Critical - 5"};

    ImGui::Combo("Log Level Filter", &m_filter_level, filter_choices, IM_ARRAYSIZE(filter_choices));
    ImGui::Separator();
    if (ImGui::BeginChild("Scrolling", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false,
                          ImGuiWindowFlags_HorizontalScrollbar))
    {
        for (const auto &line : t_wrapper.logs)
        {
            if (static_cast<int>(line.level) >= m_filter_level)
            {
                const std::filesystem::path file_path{line.source.file};

                const std::string text =
                    fmt::format("[{:%a %H:%M:%S}] [{}] [{}:{}] {}", t_wrapper.time, line.levelName(),
                                file_path.filename(), line.source.line, line.text);

                ImGui::TextColored(line.color(), "%s", text.c_str());
            }
        }
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
}

void LogMenu::draw(const Common::Debug::Wrapper &t_wrapper)
{
    auto main_window_height = GetScreenHeight();
    auto main_window_width  = GetScreenWidth();

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
    if (((main_window_width - 650.) * 0.77) >= main_window_height - 200.)
    {
        main_window_width = (main_window_height - 200.) / 0.77 + 650.;
    }
    ImGui::SetNextWindowPos(ImVec2(250, (main_window_width - 650.) * 0.77));
    ImGui::SetNextWindowSize(
        ImVec2(main_window_width - 650., main_window_height - ((main_window_width - 650.) * 0.77)));
    if (ImGui::Begin("Log", nullptr, window_flags))
    {
        drawTerminal(t_wrapper);
        ImGui::End();
    }
}
} // namespace Tyr::Gui