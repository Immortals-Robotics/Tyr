#include "config_menu.h"

namespace Tyr::Gui
{
ConfigMenu::ConfigMenu(ImVec2 _wsize)
    : menuWidth(300.), menuWidthMinimized(20.), networkNeedsUpdate(NetworkInput::None), visionIpAddress("224.5.23.2"),
      visionPort("10006")
{
    wsize       = _wsize;
    windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
}

int ConfigMenu::HandleVisionIpChange(ImGuiInputTextCallbackData *_data)
{
    static const std::regex ipRegex("^(?:[0-9]{1,3}\\.){3}[0-9]{1,3}$");

    if (std::regex_match(_data->Buf, ipRegex))
    {
        ((ConfigMenu *) _data->UserData)->SetNetworkInput(_data->Buf, NetworkInput::VisionIp);
    }
    return 0;
}

int ConfigMenu::HandleVisionPortChange(ImGuiInputTextCallbackData *_data)
{
    static const std::regex portRegex("^[1-9][0-9]*$");
    if (std::regex_match(_data->Buf, portRegex))
    {
        ((ConfigMenu *) _data->UserData)->SetNetworkInput(_data->Buf, NetworkInput::VisionPort);
    }
    return 0;
}

void ConfigMenu::SetNetworkInput(std::string _data, NetworkInput _inputType)
{
    switch (_inputType)
    {
    case NetworkInput::VisionIp:
        if (_data != this->visionIpAddress)
        {
            this->visionIpAddress    = _data;
            this->networkNeedsUpdate = NetworkInput::VisionIp;
        }
        break;
    case NetworkInput::VisionPort:
        if (_data != this->visionPort)
        {
            this->visionPort         = _data;
            this->networkNeedsUpdate = NetworkInput::VisionPort;
        }
        break;
    default:
        break;
    }
}

std::string ConfigMenu::GetNetworkParam(NetworkInput _inputType)
{
    switch (_inputType)
    {
    case NetworkInput::VisionIp:
        return this->visionIpAddress;
        break;
    case NetworkInput::VisionPort:
        return this->visionPort;
        break;
    default:
        return "";
        break;
    }
}

void ConfigMenu::DrawNetworkTab()
{
    static char visionIp[16]  = "224.5.23.2";
    static char visionPort[6] = "10006";

    ImGui::InputText("Vision Ip", visionIp, IM_ARRAYSIZE(visionIp), ImGuiInputTextFlags_CallbackAlways,
                     &ConfigMenu::HandleVisionIpChange, this);
    ImGui::InputText("Vision Port", visionPort, IM_ARRAYSIZE(visionPort), ImGuiInputTextFlags_CallbackAlways,
                     &ConfigMenu::HandleVisionPortChange, this);
}

void ConfigMenu::DrawTabBar()
{
    if (ImGui::BeginTabBar("Config tabs"))
    {
        if (ImGui::BeginTabItem("Network"))
        {
            DrawNetworkTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Tab 2"))
        {
            ImGui::Text("This is tab 2.");
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

void ConfigMenu::Draw()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(menuWidth, 700), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Config", nullptr, windowFlags))
    {
        DrawTabBar();
        ImGui::End();
    }
}

NetworkInput ConfigMenu::IsNetworkDataUpdated()
{
    return this->networkNeedsUpdate;
}

void ConfigMenu::UpdateNetworkData()
{
    this->networkNeedsUpdate = NetworkInput::None;
}

ConfigMenu::~ConfigMenu()
{}
} // namespace Tyr::Gui
