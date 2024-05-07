#pragma once

namespace Tyr::Gui
{
enum class LogState{
    NONE = 0,
    RECORD,
    PLAYBACK
};

class DemoMenu
{
private:
char m_file_name[100] = "";
char m_record_button[7] ="\uf111";
float m_playback_time = 0.;
float m_playback_size = 100.;
LogState m_log_state = LogState::NONE;

public:
    DemoMenu();
    ~DemoMenu() = default;
    void draw();
    bool isDemoRecording();
};

} // namespace Tyr::Gui