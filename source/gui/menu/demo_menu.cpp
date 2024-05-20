#include "demo_menu.h"
namespace Tyr::Gui
{
DemoMenu::DemoMenu()
{
    m_debug_storage.open(Common::setting().debug_db);
    m_world_filtered_storage.open(Common::setting().world_state_db);
    m_referee_storage.open(Common::setting().referee_db);
}

DemoMenu::~DemoMenu()
{
    m_debug_storage.close();
    m_referee_storage.close();
    m_world_filtered_storage.close();
}

void DemoMenu::draw()
{
    auto main_window_height = GetScreenHeight();
    auto main_window_width  = GetScreenWidth();

    m_real_time = Common::TimePoint::now();

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoDecoration;
    if (((main_window_width - 650.) * 0.77) >= main_window_height - 200.)
    {
        main_window_width = (main_window_height - 200.) / 0.77 + 650.;
    }
    ImGui::SetNextWindowPos(ImVec2(main_window_width - 400., 350.));
    ImGui::SetNextWindowSize(ImVec2(GetScreenWidth() - main_window_width + 400, 200.));
    if (ImGui::Begin("Demo player", nullptr, window_flags))
    {
        ImGuiIO &io   = ImGui::GetIO();
        ImFont  *font = io.Fonts->Fonts[0]; // Change the index if you have multiple fonts and want a different one
        font->Scale   = 2;
        ImGui::PushFont(font);
        if (ImGui::Button("\uf07c", {100, 40}))
        {
            analyzeDatabase();
        }

        if (!m_demos.empty())
        {
            ImGui::SameLine();
            font->Scale = 1;
            ImGui::PushFont(font);

            if (ImGui::Combo(
                    "##DemoList", &m_selected_demo, [](void *user_data, int idx) -> const char *
                    { return ((Demo *) user_data + idx)->name.c_str(); }, m_demos.data(), m_demos.size()))
            {
                m_log_state = LogState::None;
                m_play_time = currentDemo().start_time;
            }
            ImGui::Spacing();
            ImGui::Spacing();
            font->Scale = 1.8;
            ImGui::PushFont(font);
            if (ImGui::Button("\uf04a", {40, 40}))
            {
                if (m_log_state != LogState::None &&
                    m_play_time > m_play_time + Common::Duration::fromMicroseconds(100))
                {
                    m_play_time -= Common::Duration::fromMicroseconds(100);
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("\uf04b", {40, 40}))
            {
                if (m_log_state != LogState::PlaybackPlay)
                {
                    m_log_state = LogState::PlaybackPlay;

                    m_play_time = std::clamp(m_play_time, currentDemo().start_time, currentDemo().end_time);

                    m_play_start_time = m_real_time - (m_play_time - currentDemo().start_time);
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("\uf04c", {40, 40}))
            {
                if (m_log_state == LogState::PlaybackPlay)
                {
                    m_log_state = LogState::PlaybackPause;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("\uf04d", {40, 40}))
            {
                if (m_log_state != LogState::None)
                {
                    m_log_state = LogState::None;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("\uf04e", {40, 40}))
            {
                if (m_log_state != LogState::None)
                {
                    m_play_time += Common::Duration::fromMicroseconds(100);
                }
            }
            ImGui::SameLine();
            ImGui::Spacing();
            ImGui::SameLine();
            if (ImGui::Button(m_record_button, {40, 40}))
            {
                if (m_log_state != LogState::Record)
                {
                    m_log_state = LogState::Record;
                    strcpy(m_record_button, "\uf28d");
                }
                else
                {
                    m_log_state = LogState::None;
                    strcpy(m_record_button, "\uf111");
                }
            }

            ImGui::Spacing();

            font->Scale = 1;
            ImGui::PushFont(font);

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Spacing();

            float elapsed_sec = m_playback_elapsed.seconds();
            ImGui::SetNextItemWidth(390.);
            if (ImGui::SliderFloat("##time", &elapsed_sec, 0.0f, currentDemo().length().seconds(), "%.3fs") &&
                m_demos.size())
            {
                m_playback_elapsed = Common::Duration::fromSeconds(elapsed_sec);
                m_play_time        = currentDemo().start_time + m_playback_elapsed;
            }
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.7f, 0.25f, 0.3f, 1.0f));        // Dark frame background
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.7f, 0.35f, 0.4f, 1.0f)); // Darker when hovered
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));   // Brighter red when active

            ImGui::SetNextItemWidth(390.);
            elapsed_sec = m_playback_elapsed.seconds();
            if (ImGui::DragFloat("##timescrol", &elapsed_sec, 0.01f, 0.0f, currentDemo().length().seconds(),
                                 "Fine Control"))
            {
                m_playback_elapsed = Common::Duration::fromSeconds(elapsed_sec);
                m_play_time        = currentDemo().start_time + m_playback_elapsed;
            }
            ImGui::PopStyleColor(3);

            ImGui::PopFont();
            ImGui::PopFont();
            ImGui::PopFont();
        }

        ImGui::PopFont();
        font->Scale = 1;

        ImGui::End();
    }
    demoHandler();
}

LogState DemoMenu::getState()
{
    return m_log_state;
}

void DemoMenu::demoHandler()
{
    switch (m_log_state)
    {
    case LogState::PlaybackPlay:
        m_playback_elapsed = m_real_time - m_play_start_time;

        m_play_time = currentDemo().start_time + m_playback_elapsed;
        m_play_time = std::min(m_play_time, currentDemo().end_time);

    case LogState::PlaybackPause:
        m_world_filtered_storage.get(m_play_time.microseconds(), &m_worldstate_filtered);
        m_debug_storage.get(m_play_time.microseconds(), &m_debug);
        m_referee_storage.get(m_play_time.microseconds(), &m_referee);
        break;
    }
}

void DemoMenu::analyzeDatabase()
{
    m_demos.clear();

    Common::Storage::Key start_ts = 0;
    Common::Storage::Key end_ts   = 0;

    m_world_filtered_storage.getBoundary(&start_ts, &end_ts);

    Common::Storage::Key current_start_ts = start_ts;

    for (Common::Storage::Key current_ts = start_ts; current_ts < end_ts;)
    {
        m_debug_storage.closest(current_ts, &current_ts);

        Common::Storage::Key next_ts;
        m_debug_storage.next(current_ts, &next_ts);

        if (next_ts - current_ts > 1000000)
        {
            m_demos.emplace_back(current_start_ts, current_ts);
            current_start_ts = next_ts;
        }

        current_ts = next_ts;
    }

    m_demos.emplace_back(current_start_ts, end_ts);
}
} // namespace Tyr::Gui
