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

    m_real_time = Common::TimePoint::now().microseconds();

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
        ImGui::SameLine();
        font->Scale = 1;
        ImGui::PushFont(font);
        if (ImGui::Combo("##DemoList", &m_selected_demo, m_demo_names.data(), m_demo_names.size()) &&
            m_start_times.size())
        {
            m_playback_size =
                static_cast<float>(m_end_times[m_selected_demo] - m_start_times[m_selected_demo]) / 1000000.;
            m_log_state                           = LogState::None;
            m_worldstate_filtered.info.current_ts = m_start_times[m_selected_demo];
        }
        ImGui::Spacing();
        ImGui::Spacing();
        font->Scale = 1.8;
        ImGui::PushFont(font);
        if (ImGui::Button("\uf04a", {40, 40}))
        {
            if (m_log_state != LogState::None &&
                m_worldstate_filtered.info.current_ts > m_worldstate_filtered.info.start_ts + 100)
            {
                m_worldstate_filtered.info.current_ts -= 100;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("\uf04b", {40, 40}))
        {
            if (m_log_state != LogState::PlaybackPlay)
            {
                m_log_state = LogState::PlaybackPlay;
                m_play_time = m_real_time;
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
                m_worldstate_filtered.info.current_ts += 100;
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

        ImGui::SetNextItemWidth(390.);
        if (ImGui::SliderFloat("##time", &m_playback_time, 0.0f, m_playback_size, "%.3fs") && m_start_times.size())
        {
            m_worldstate_filtered.info.current_ts =
                static_cast<Common::Storage::Key>(m_playback_time * 1000000) + m_start_times[m_selected_demo];
        }
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.7f, 0.25f, 0.3f, 1.0f));        // Dark frame background
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.7f, 0.35f, 0.4f, 1.0f)); // Darker when hovered
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));   // Brighter red when active

        ImGui::SetNextItemWidth(390.);
        if (ImGui::DragFloat("##timescrol", &m_playback_time, 0.01f, 0.0f, m_playback_size, "Fine Control"))
        {
            m_worldstate_filtered.info.current_ts =
                static_cast<Common::Storage::Key>(m_playback_time * 1000000) + m_start_times[m_selected_demo];
        }
        ImGui::PopStyleColor(3);

        ImGui::PopFont();
        ImGui::PopFont();
        ImGui::PopFont();
        ImGui::PopFont();

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
    Common::Storage::Key next_ts;
    switch (m_log_state)
    {
    case LogState::PlaybackPlay:
        if (m_real_time - m_play_time >= m_worldstate_filtered.info.time_between_frames &&
            m_worldstate_filtered.info.current_ts < m_end_times[m_selected_demo] && m_start_times.size())
        {
            if (m_world_filtered_storage.closest(m_worldstate_filtered.info.current_ts,
                                                 &m_worldstate_filtered.info.current_ts, &next_ts))
            {
                m_worldstate_filtered.info.time_between_frames = next_ts - m_worldstate_filtered.info.current_ts;
                m_play_time += m_worldstate_filtered.info.time_between_frames;
                m_worldstate_filtered.info.current_ts = next_ts;
                m_playback_time =
                    static_cast<float>(m_worldstate_filtered.info.current_ts - m_start_times[m_selected_demo]) /
                    1000000;

                Protos::Immortals::WorldState world_filtered;
                if (m_world_filtered_storage.get(m_worldstate_filtered.info.current_ts, &world_filtered))
                {
                    m_worldstate_filtered.message = world_filtered;
                }

                Protos::Immortals::Debug::Wrapper debug_message;
                if (m_debug_storage.get(m_worldstate_filtered.info.current_ts, &debug_message))
                {
                    m_debug.message = debug_message;
                }

                Protos::Immortals::RefereeState referee_message;
                if (m_referee_storage.get(m_worldstate_filtered.info.current_ts, &referee_message))
                {
                    m_referee.message = referee_message;
                }
            }
        }
        break;
    case LogState::PlaybackPause:
        if (m_world_filtered_storage.closest(m_worldstate_filtered.info.current_ts,
                                             &m_worldstate_filtered.info.current_ts, &next_ts))
        {
            Protos::Immortals::WorldState world_filtered;
            if (m_world_filtered_storage.get(m_worldstate_filtered.info.current_ts, &world_filtered))
            {
                m_worldstate_filtered.message = world_filtered;
            }

            Protos::Immortals::Debug::Wrapper debug_message;
            if (m_debug_storage.get(m_worldstate_filtered.info.current_ts, &debug_message))
            {
                m_debug.message = debug_message;
            }

            Protos::Immortals::RefereeState referee_message;
            if (m_referee_storage.get(m_worldstate_filtered.info.current_ts, &referee_message))
            {
                m_referee.message = referee_message;
            }
        }
        break;
    default:
        break;
    }
}

void DemoMenu::analyzeDatabase()
{
    m_debug_storage.getBoundary(&m_debug.info.start_ts, &m_debug.info.end_ts);
    m_world_filtered_storage.getBoundary(&m_worldstate_filtered.info.start_ts, &m_worldstate_filtered.info.end_ts);
    m_referee_storage.getBoundary(&m_referee.info.start_ts, &m_referee.info.end_ts);

    m_debug.info.current_ts               = m_debug.info.start_ts;
    m_worldstate_filtered.info.current_ts = m_worldstate_filtered.info.start_ts;
    m_referee.info.current_ts             = m_referee.info.start_ts;

    m_start_times.clear();
    m_demo_names.clear();
    m_end_times.clear();

    pushStartPoints(m_worldstate_filtered.info.current_ts);

    Common::Storage::Key next_ts;
    while (m_worldstate_filtered.info.current_ts < m_worldstate_filtered.info.end_ts)
    {
        m_debug_storage.closest(m_worldstate_filtered.info.current_ts, &m_worldstate_filtered.info.current_ts,
                                &next_ts);
        if (next_ts - m_worldstate_filtered.info.current_ts > 1000000)
        {
            m_end_times.push_back(m_worldstate_filtered.info.current_ts);
            pushStartPoints(next_ts);
        }
        m_worldstate_filtered.info.current_ts = next_ts;
    }
    m_end_times.push_back(m_worldstate_filtered.info.end_ts);

    m_playback_size                       = static_cast<float>(m_end_times[0] - m_start_times[0]) / 1000000.;
    m_worldstate_filtered.info.current_ts = m_start_times[0];
}

void DemoMenu::pushStartPoints(Common::Storage::Key t_ts)
{
    auto               tp         = std::chrono::system_clock::time_point(std::chrono::microseconds(t_ts));
    auto               time       = std::chrono::system_clock::to_time_t(tp);
    auto              *local_time = std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(local_time, "%Y/%m/%d-%H:%M:%S");
    m_start_times.push_back(t_ts);

    char *c_str = new char[oss.str().length() + 1];
    strcpy(c_str, oss.str().c_str());
    m_demo_names.push_back(c_str);
}
} // namespace Tyr::Gui
