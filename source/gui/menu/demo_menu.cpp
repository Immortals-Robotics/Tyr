#include "demo_menu.h"

namespace Tyr::Gui
{
DemoMenu::DemoMenu()
{}

void DemoMenu::draw()
{
    auto     main_window_height = GetScreenHeight();
    auto     main_window_width  = GetScreenWidth();
    auto     now                = std::chrono::high_resolution_clock::now();
    long int timestamp_ms       = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoDecoration;

    ImGui::SetNextWindowPos(ImVec2(main_window_width - 400., 400.));
    ImGui::SetNextWindowSize(ImVec2(400., main_window_height - 400.));
    if (ImGui::Begin("Demo player", nullptr, window_flags))
    {
        ImGuiIO &io       = ImGui::GetIO();
        ImFont  *font     = io.Fonts->Fonts[0]; // Change the index if you have multiple fonts and want a different one
        float    fontSize = 24.0f;              // Set the desired font size
        font->Scale       = 2;
        ImGui::PushFont(font);
        ImGui::Button("\uf07c", {100, 40});
        ImGui::SameLine();
        font->Scale = 1;
        ImGui::PushFont(font);
        ImGui::Text("Demo name");
                ImGui::Spacing();
        ImGui::Spacing();

        font->Scale = 1.8;
        ImGui::PushFont(font);
        ImGui::Button("\uf04a", {40, 40});
        ImGui::SameLine();
        ImGui::Button("\uf04b", {40, 40});
        ImGui::SameLine();
        ImGui::Button("\uf04c", {40, 40});
        ImGui::SameLine();
        ImGui::Button("\uf04d", {40, 40});
        ImGui::SameLine();
        ImGui::Button("\uf04e", {40, 40});
        ImGui::SameLine();
        ImGui::Spacing();
        ImGui::SameLine();
        if (ImGui::Button(m_record_button, {40, 40}))
        {
            if (m_log_state == LogState::NONE || m_log_state == LogState::PLAYBACK)
            {
                m_log_state = LogState::RECORD;
                strcpy(m_record_button, "\uf28d");
            }
            else
            {
                m_log_state = LogState::NONE;
                strcpy(m_record_button, "\uf111");
            }
        }

        ImGui::Spacing();

        font->Scale = 1;
        ImGui::PushFont(font);

        if (m_log_state != LogState::RECORD)
        {
            ImGui::InputText("Demo name", m_file_name, IM_ARRAYSIZE(m_file_name));
        }
        else
        {
            ImGui::Text("   Recording to name ");
        }
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::SetNextItemWidth(390.);
        ImGui::SliderFloat("##time", &m_playback_time, 0.0f, m_playback_size, "%.3fs");
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.7f, 0.25f, 0.3f, 1.0f));        // Dark frame background
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.7f, 0.35f, 0.4f, 1.0f)); // Darker when hovered
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));   // Brighter red when active

        ImGui::SetNextItemWidth(390.);
        ImGui::DragFloat("##timescrol", &m_playback_time, 0.01f, 0.0f, m_playback_size, "Fine Control");
        ImGui::PopStyleColor(5);

        ImGui::PopFont();
        ImGui::End();
    }
}
bool DemoMenu::isDemoRecording()
{
    return m_log_state == LogState::RECORD;
}

} // namespace Tyr::Gui