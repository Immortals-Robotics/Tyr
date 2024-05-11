#pragma once

namespace Tyr::Gui
{
enum class LogState
{
    None = 0,
    Record,
    PlaybackPlay
};

class DemoMenu
{
private:
    void demoHandler();
    void initStorage(std::string_view t_name);
    void analyzeDatabase();
    void pushStartPoints(unsigned long t_ts);

    char                              m_file_name[100]   = "";
    char                              m_record_button[7] = "\uf111";
    float                             m_playback_time    = 0.;
    float                             m_playback_size    = 100.;
    LogState                          m_log_state        = LogState::None;
    Protos::Immortals::Debug::Wrapper m_debug_pb_wrapper;
    unsigned long                     m_play_time = 0;
    unsigned long                     m_real_time = 0;
    unsigned long                     m_first_ts, m_last_ts;
    unsigned long                     m_current_ts = 0, m_next_ts = 0;
    unsigned long                     m_time_between_frames = 0;
    std::vector<unsigned long>        m_start_times;
    std::vector<unsigned long>        m_end_times;

    int                       m_selected_demo = 0;
    std::vector<const char *> m_demo_names;

    Common::Storage m_storage;
    std::mutex      m_demo_mutex;

public:
    DemoMenu() = default;
    ~DemoMenu();
    void     draw();
    LogState getState();

    const Protos::Immortals::Debug::Wrapper &debugWrapper() const
    {
        return m_debug_pb_wrapper;
    }
};

} // namespace Tyr::Gui