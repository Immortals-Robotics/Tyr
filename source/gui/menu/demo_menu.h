#pragma once

namespace Tyr::Gui
{
enum class LogState
{
    None = 0,
    Record,
    PlaybackPlay,
    PlaybackPause
};

class DemoMenu
{
private:
    void demoHandler();
    void analyzeDatabase();

    char     m_record_button[7] = "\uf111";
    float    m_playback_time    = 0.;
    float    m_playback_size    = 100.;
    LogState m_log_state        = LogState::None;

    Common::Storage::Key m_play_time       = 0;
    Common::Storage::Key m_play_start_time = 0;
    Common::Storage::Key m_real_time       = 0;

    struct Demo
    {
        Common::Storage::Key start_time;
        Common::Storage::Key end_time;
        std::string          name;
    };

    inline const Demo &currentDemo() const
    {
        return m_demos[m_selected_demo];
    }

    std::vector<Demo> m_demos;
    int               m_selected_demo = 0;

    Common::Storage m_debug_storage;
    Common::Storage m_world_filtered_storage;
    Common::Storage m_referee_storage;

    Protos::Immortals::Debug::Wrapper m_debug;
    Protos::Immortals::WorldState     m_worldstate_filtered;
    Protos::Immortals::RefereeState   m_referee;

public:
    DemoMenu();
    ~DemoMenu();
    void     draw();
    LogState getState();

    const Protos::Immortals::Debug::Wrapper &debugWrapper() const
    {
        return m_debug;
    }

    const Protos::Immortals::WorldState &worldStateFiltered() const
    {
        return m_worldstate_filtered;
    }

    const Protos::Immortals::RefereeState &refereeState() const
    {
        return m_referee;
    }
};

} // namespace Tyr::Gui
