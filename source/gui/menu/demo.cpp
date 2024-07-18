#include "demo.h"
namespace Tyr::Gui
{
DemoMenu::DemoMenu()
{
    m_debug_storage.open(Common::config().network.debug_db);
    m_world_filtered_storage.open(Common::config().network.world_state_db);
    m_referee_storage.open(Common::config().network.referee_db);
    m_soccer_storage.open(Common::config().network.soccer_db);
}

DemoMenu::~DemoMenu()
{
    m_debug_storage.close();
    m_world_filtered_storage.close();
    m_referee_storage.close();
    m_soccer_storage.close();
}

void DemoMenu::draw()
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
                "##DemoList", &m_selected_demo, [](void *user_data, const int idx) -> const char *
                { return (static_cast<Demo *>(user_data) + idx)->name.c_str(); }, m_demos.data(), m_demos.size()))
        {
            m_timer.reset();
        }
        ImGui::Spacing();
        ImGui::Spacing();
        font->Scale = 1.8f;
        ImGui::PushFont(font);
        if (ImGui::Button("\uf04a", {40, 40}))
        {
            if (m_log_state == LogState::Replay && m_timer.time() > Common::Duration::fromMilliseconds(100))
            {
                m_timer.setTime(m_timer.time() - Common::Duration::fromMilliseconds(100));
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("\uf04b", {40, 40}))
        {
            if (m_log_state == LogState::Replay)
            {
                m_timer.resume();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("\uf04c", {40, 40}))
        {
            if (m_log_state == LogState::Replay)
            {
                m_timer.pause();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("\uf04d", {40, 40}))
        {
            if (m_log_state == LogState::Replay)
            {
                m_timer.reset();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("\uf04e", {40, 40}))
        {
            if (m_log_state == LogState::Replay)
            {
                m_timer.setTime(m_timer.time() + Common::Duration::fromMilliseconds(100));
            }
        }
        ImGui::SameLine();
        ImGui::Spacing();
        ImGui::SameLine();
        if (ImGui::Button(m_record_button, {40, 40}))
        {
            if (m_log_state != LogState::Live)
            {
                m_log_state     = LogState::Live;
                m_record_button = kRecordButtonLive;

                m_timer.reset();
            }
            else
            {
                m_log_state     = LogState::Replay;
                m_record_button = kRecordButtonReplay;
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

        Common::Duration playback_time = m_timer.time();
        float            playback_sec  = playback_time.seconds();

        ImGui::SetNextItemWidth(390.);
        if (ImGui::SliderFloat("##time", &playback_sec, 0.0f, currentDemo().length().seconds(), "%.3fs") &&
            m_demos.size())
        {
            m_timer.setTime(Common::Duration::fromSeconds(playback_sec));
        }
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.7f, 0.25f, 0.3f, 1.0f));        // Dark frame background
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.7f, 0.35f, 0.4f, 1.0f)); // Darker when hovered
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));   // Brighter red when active

        ImGui::SetNextItemWidth(390.);

        if (ImGui::DragFloat("##timescrol", &playback_sec, 0.01f, 0.0f, currentDemo().length().seconds(),
                             "Fine Control"))
        {
            m_timer.setTime(Common::Duration::fromSeconds(playback_sec));
        }
        ImGui::PopStyleColor(3);

        ImGui::PopFont();
        ImGui::PopFont();
        ImGui::PopFont();
    }

    ImGui::PopFont();
    font->Scale = 1;
}

void DemoMenu::update()
{
    if (m_log_state != LogState::Replay)
        return;

    if (!m_timer.paused() && m_timer.time() >= currentDemo().length())
    {
        m_timer.pause();
        m_timer.setTime(currentDemo().length());
    }

    const Common::TimePoint playback_point = currentDemo().start_time + m_timer.time();

    Protos::Immortals::WorldState     pb_world_state;
    Protos::Immortals::Debug::Wrapper pb_debug;
    Protos::Immortals::Referee::State pb_referee;
    Protos::Immortals::Soccer::State  pb_soccer;

    m_world_filtered_storage.get(playback_point.microseconds(), &pb_world_state);
    m_debug_storage.get(playback_point.microseconds(), &pb_debug);
    m_referee_storage.get(playback_point.microseconds(), &pb_referee);
    m_soccer_storage.get(playback_point.microseconds(), &pb_soccer);

    m_world_state   = static_cast<Common::WorldState>(pb_world_state);
    m_debug         = static_cast<Common::Debug::Wrapper>(pb_debug);
    m_referee_state = static_cast<Common::Referee::State>(pb_referee);
    m_soccer_state  = static_cast<Common::Soccer::State>(pb_soccer);
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
