#pragma once

namespace Tyr::Gui
{
class ConfigCallback;
const int max_num_of_cameras = 8;
enum class InputCallbackType
{
    None = 0,
    VISION_IP,
    VISION_PORT,
    REF_IP,
    REF_PORT,
    SENDER_IP,
    SENDER_PORT,
    STRATEGY_IP,
    STRATEGY_PORT,
    TRACKER_IP,
    TRACKER_PORT,
    GRSIM_IP,
    GRSIM_PORT,
};

const std::map<InputCallbackType, std::string> config_input_map{
    {InputCallbackType::None, ""},
    {InputCallbackType::VISION_IP, "network.vision.ip"},
    {InputCallbackType::VISION_PORT, "network.vision.port"},
    {InputCallbackType::REF_IP, "network.referee.ip"},
    {InputCallbackType::REF_PORT, "network.referee.port"},
    {InputCallbackType::SENDER_IP, "network.sender.ip"},
    {InputCallbackType::SENDER_PORT, "network.sender.port"},
    {InputCallbackType::STRATEGY_IP, "network.strategy.ip"},
    {InputCallbackType::STRATEGY_PORT, "network.strategy.port"},
    {InputCallbackType::TRACKER_IP, "network.tracker.ip"},
    {InputCallbackType::TRACKER_PORT, "network.tracker.port"},
    {InputCallbackType::GRSIM_IP, "network.grsim.ip"},
    {InputCallbackType::GRSIM_PORT, "network.grsim.port"},
};

class ConfigMenu
{
private:
    ImGuiWindowFlags  m_window_flags;
    InputCallbackType m_network_needs_update;

    char m_vision_ip_text[16]    = "224.5.23.2";
    char m_vision_port_text[6]   = "10006";
    char m_referee_ip_text[16]   = "224.5.23.1";
    char m_referee_port_text[6]  = "10003";
    char m_sender_ip_text[16]    = "224.5.23.2";
    char m_sender_port_text[6]   = "10011";
    char m_strategy_ip_text[16]  = "224.5.23.3";
    char m_strategy_port_text[6] = "60006";
    char m_tracker_ip_text[16]   = "224.5.23.2";
    char m_tracker_port_text[6]  = "10099";
    char m_grsim_ip_text[16]     = "127.0.0.1";
    char m_grsim_port_text[6]    = "20011";

    char m_ball_merge_distance_text[10];
    char m_max_ball_2_frame_dist[10];
    char m_max_ball_frame_not_seen[10];
    char m_max_ball_hist[10];
    char m_max_robot_frame_not_seen[10];
    char m_merge_distance[10];
    char m_vision_frame_rate[10];
    Common::Tree<std::string> filterTree;

    std::array<bool, max_num_of_cameras> m_use_camera     = {true, true, false};
    bool                                 m_use_kalman_ang = false;
    bool                                 m_use_kalman_pos = true;

    const std::map<InputCallbackType, char *> m_type_input_text_map{
        {InputCallbackType::VISION_IP, m_vision_ip_text},     {InputCallbackType::VISION_PORT, m_vision_port_text},
        {InputCallbackType::REF_IP, m_referee_ip_text},       {InputCallbackType::REF_PORT, m_referee_port_text},
        {InputCallbackType::SENDER_IP, m_sender_ip_text},     {InputCallbackType::SENDER_PORT, m_sender_port_text},
        {InputCallbackType::STRATEGY_IP, m_strategy_ip_text}, {InputCallbackType::STRATEGY_PORT, m_strategy_port_text},
        {InputCallbackType::TRACKER_IP, m_tracker_ip_text},   {InputCallbackType::TRACKER_PORT, m_tracker_port_text},
        {InputCallbackType::GRSIM_IP, m_grsim_ip_text},       {InputCallbackType::GRSIM_PORT, m_grsim_port_text},
    };

    void drawTabBar();
    void drawNetworkTab();
    void drawConfigTab();
    void drawFilterTab();
    void drawIpPortInput(const std::string _name, const int _id, char *_ip_text, char *_port_text,
                         ConfigCallback &_callback, const InputCallbackType _callback_type_ip,
                         const InputCallbackType _callback_type_port);

    static int handleInputChange(ImGuiInputTextCallbackData *_data);

public:
    ConfigMenu();
    ~ConfigMenu() = default;
    void receiveDebug(const Common::Debug::Wrapper &t_wrapper);
    void setNetworkInput(std::string _data, InputCallbackType _inputType);

    std::string       getNetworkParam(InputCallbackType _inputType);
    InputCallbackType isNetworkDataUpdated();
    void              updateNetworkData();
    void              draw();
};

class ConfigCallback
{
private:
    ConfigMenu       *m_menu;
    InputCallbackType m_callback_type;
    std::regex        m_regex;

public:
    ConfigCallback(ConfigMenu *_menu);
    ~ConfigCallback() = default;
    void             setParams(const InputCallbackType _callback_type, const std::regex _regex);
    const std::regex getRegex();
    void             executeCallback(std::string _data);
};
} // namespace Tyr::Gui
