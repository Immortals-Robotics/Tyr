#pragma once

namespace Tyr::Gui
{
enum class NetworkInput
{
    None = 0,
    VISION_IP,
    VISION_PORT,
    REF_IP,
    REF_PORT,
};

class ConfigMenu
{
private:
    const float      menuWidth;
    const float      menuWidthMinimized;
    ImGuiWindowFlags windowFlags;
    NetworkInput     networkNeedsUpdate;

    char vision_ip_text[16];
    char vision_port_text[6];
    char referee_ip_text[16];
    char referee_port_text[6];

    void DrawTabBar();
    void DrawNetworkTab();
    void SetNetworkInput(std::string _data, NetworkInput _inputType);
    void drawIpPortInput(std::string _name, int _id, char *_ip_text, char *_port_text,
                         int (*_callback_ip)(ImGuiInputTextCallbackData *),
                         int (*_callback_port)(ImGuiInputTextCallbackData *));

    static int HandleVisionIpChange(ImGuiInputTextCallbackData *_data);
    static int HandleVisionPortChange(ImGuiInputTextCallbackData *_data);
    static int HandleRefereeIpChange(ImGuiInputTextCallbackData *_data);
    static int HandleRefereePortChange(ImGuiInputTextCallbackData *_data);

public:
    ConfigMenu();
    ~ConfigMenu();

    std::string  GetNetworkParam(NetworkInput _inputType);
    NetworkInput IsNetworkDataUpdated();
    void         UpdateNetworkData();
    void         Draw();
};
} // namespace Tyr::Gui
