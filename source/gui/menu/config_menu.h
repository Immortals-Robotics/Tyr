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
    ImGuiWindowFlags windowFlags;
    std::string      visionIpAddress;
    std::string      visionPort;
    NetworkInput     networkNeedsUpdate;

    void DrawTabBar();
    void DrawNetworkTab();
    void SetNetworkInput(std::string _data, NetworkInput _inputType);

    static int HandleVisionIpChange(ImGuiInputTextCallbackData *_data);
    static int HandleVisionPortChange(ImGuiInputTextCallbackData *_data);

public:
    ConfigMenu();
    ~ConfigMenu();

    std::string  GetNetworkParam(NetworkInput _inputType);
    NetworkInput IsNetworkDataUpdated();
    void         UpdateNetworkData();
    void         Draw();
};
} // namespace Tyr::Gui
