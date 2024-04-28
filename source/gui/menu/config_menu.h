#pragma once

namespace Tyr::Gui
{
typedef enum
{
    VISION_IP,
    VISION_PORT,
    REF_IP,
    REF_PORT,
    NETWORK_NONE
} networkInput_t;

class ConfigMenu
{
private:
    const float      menuWidth;
    const float      menuWidthMinimized;
    ImVec2           wsize;
    ImGuiWindowFlags windowFlags;
    std::string      visionIpAddress;
    std::string      visionPort;
    networkInput_t   networkNeedsUpdate;

    void       DrawTabBar(void);
    void       DrawNetworkTab(void);
    static int HandleVisionIpChange(ImGuiInputTextCallbackData *_data);
    static int HandleVisionPortChange(ImGuiInputTextCallbackData *_data);
    void       SetNetworkInput(std::string _data, networkInput_t _inputType);

public:
    ConfigMenu(ImVec2 _wsize);
    std::string    GetNetworkParam(networkInput_t _inputType);
    networkInput_t IsNetworkDataUpdated();
    void           UpdateNetworkData();
    void           Draw(void);
    ~ConfigMenu();
};
} // namespace Tyr::Gui
