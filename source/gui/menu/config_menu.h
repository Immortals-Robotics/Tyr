#pragma once

namespace Tyr::Gui
{
enum class NetworkInput
{
    None = 0,
    VisionIp,
    VisionPort,
    RefereeIp,
    RefereePort,
};

class ConfigMenu
{
private:
    const float      menuWidth;
    const float      menuWidthMinimized;
    ImVec2           wsize;
    ImGuiWindowFlags windowFlags;
    std::string      visionIpAddress;
    std::string      visionPort;
    NetworkInput     networkNeedsUpdate;

    void DrawTabBar(void);
    void DrawNetworkTab(void);
    void SetNetworkInput(std::string _data, NetworkInput _inputType);

    static int HandleVisionIpChange(ImGuiInputTextCallbackData *_data);
    static int HandleVisionPortChange(ImGuiInputTextCallbackData *_data);

public:
    ConfigMenu(ImVec2 _wsize);
    ~ConfigMenu();

    std::string  GetNetworkParam(NetworkInput _inputType);
    NetworkInput IsNetworkDataUpdated();
    void         UpdateNetworkData();
    void         Draw(void);
};
} // namespace Tyr::Gui
