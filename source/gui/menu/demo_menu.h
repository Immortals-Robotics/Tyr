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
    void initStorage();
    void analyzeDatabase();
    void pushStartPoints(Common::Storage::Key t_ts);

    struct Info
    {
        Common::Storage::Key start_ts;
        Common::Storage::Key end_ts;
        Common::Storage::Key current_ts;
        Common::Storage::Key time_between_frames;
        Info() = default;
    };

    struct DebugDemo
    {
        Info                              info;
        Protos::Immortals::Debug::Wrapper message;
    };

    struct WorldSateFilteredDemo
    {
        Info                          info;
        Protos::Immortals::WorldState message;
    };

    char     m_record_button[7] = "\uf111";
    float    m_playback_time    = 0.;
    float    m_playback_size    = 100.;
    LogState m_log_state        = LogState::None;

    Common::Storage::Key              m_play_time = 0;
    Common::Storage::Key              m_real_time = 0;
    std::vector<Common::Storage::Key> m_start_times;
    std::vector<Common::Storage::Key> m_end_times;

    int                       m_selected_demo = 0;
    std::vector<const char *> m_demo_names;

    Common::Storage m_storage;
    Common::Storage m_world_filtered_storage;

    DebugDemo             m_debug;
    WorldSateFilteredDemo m_worldstate_filtered;

public:
    DemoMenu() = default;
    ~DemoMenu();
    void     draw();
    LogState getState();

    const Protos::Immortals::Debug::Wrapper &debugWrapper() const
    {
        return m_debug.message;
    }

    const Protos::Immortals::WorldState &worldStateFiltered() const
    {
        return m_worldstate_filtered.message;
    }
};

} // namespace Tyr::Gui