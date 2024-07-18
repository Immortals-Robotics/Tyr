#pragma once

namespace Tyr::Gui
{
enum class LogState
{
    Live = 0,
    Replay,
};

class DemoMenu
{
public:
     DemoMenu();
    ~DemoMenu();

    void update();
    void draw();

    LogState getState() const
    {
        return m_log_state;
    }

    const Common::Debug::Wrapper &debugWrapper() const
    {
        return m_debug;
    }

    const Common::WorldState &worldState() const
    {
        return m_world_state;
    }

    const Common::Referee::State &refereeState() const
    {
        return m_referee_state;
    }

    const Common::Soccer::State &soccerState() const
    {
        return m_soccer_state;
    }

private:
    void analyzeDatabase();

    const char *kRecordButtonLive   = "\uf28d";
    const char *kRecordButtonReplay = "\uf111";

    const char *m_record_button = kRecordButtonLive;

    LogState m_log_state = LogState::Live;

    Common::Timer m_timer;

    struct Demo
    {
        Common::TimePoint start_time;
        Common::TimePoint end_time;
        std::string       name;

        Demo() = default;

        Demo(const Common::Storage::Key t_start_ts, const Common::Storage::Key t_end_ts)
        {
            start_time = Common::TimePoint::fromMicroseconds(t_start_ts);
            end_time   = Common::TimePoint::fromMicroseconds(t_end_ts);
            name       = fmt::format("{}", start_time);
        }

        Common::Duration length() const
        {
            return end_time - start_time;
        }
    };

    const Demo &currentDemo() const
    {
        return m_demos[m_selected_demo];
    }

    std::vector<Demo> m_demos;
    int               m_selected_demo = 0;

    Common::Storage m_debug_storage;
    Common::Storage m_world_filtered_storage;
    Common::Storage m_referee_storage;
    Common::Storage m_soccer_storage;

    Common::Debug::Wrapper m_debug;
    Common::WorldState     m_world_state;
    Common::Referee::State m_referee_state;
    Common::Soccer::State  m_soccer_state;
};

} // namespace Tyr::Gui
