#include "log_menu.h"
namespace Tyr::Gui
{

bool LogMenu::applyFilter(const Tyr::Common::Debug::Log::Level t_level)
{
    if (static_cast<int>(t_level) >= m_filter_level)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void LogMenu::drawTerminal(const Common::Debug::Wrapper &t_wrapper)
{
    const char *filter_choices[] = {"Trace - 0", "Debug - 1", "Info - 2", "Warning - 3", "Error - 4", "Critical - 5"};

    std::vector<Tyr::Common::Debug::Log> terminal_output;
    ImGui::Combo("Log Level Filter", &m_filter_level, filter_choices, IM_ARRAYSIZE(filter_choices));
    ImGui::Separator();
    for (const auto &log : t_wrapper.logs)
    {
        terminal_output.push_back(log);
    }
    if (ImGui::BeginChild("Scrolling", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false,
                          ImGuiWindowFlags_HorizontalScrollbar))
    {
        for (const auto &line : terminal_output)
        {
            if (applyFilter(line.level))
            {
                ImVec4 line_color(line.color.r / 255., line.color.g / 255., line.color.b / 255., line.color.a / 255.);
                ImGui::TextColored(line_color, "[%s] [%s] [%s:%s:%d] %s", line.logger_name.data(),
                                   log_level_map.at(line.level).c_str(), line.source.file.data(),
                                   line.source.function.data(), line.source.line, line.payload.c_str());
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