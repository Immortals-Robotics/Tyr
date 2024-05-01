#include "config_menu.h"

namespace Tyr::Gui
{
static const std::regex ipRegex(
    "(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])");
static const std::regex portRegex("^[1-9][0-9]*$");

ConfigMenu::ConfigMenu() : menuWidth(300.), menuWidthMinimized(20.), networkNeedsUpdate(NetworkInput::None)
{
    windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
    strcpy(vision_ip_text, Tyr::Common::setting().vision_address.ip.c_str());
    strcpy(vision_port_text, std::to_string(Tyr::Common::setting().vision_address.port).c_str());

    strcpy(referee_ip_text, Tyr::Common::setting().referee_address.ip.c_str());
    strcpy(referee_port_text, std::to_string(Tyr::Common::setting().referee_address.port).c_str());
}

// Vision
int ConfigMenu::HandleVisionIpChange(ImGuiInputTextCallbackData *_data)
{
    if (std::regex_match(_data->Buf, ipRegex))
    {
        ((ConfigMenu *) _data->UserData)->SetNetworkInput(_data->Buf, NetworkInput::VISION_IP);
    }
    return 0;
}

int ConfigMenu::HandleVisionPortChange(ImGuiInputTextCallbackData *_data)
{
    if (std::regex_match(_data->Buf, portRegex))
    {
        ((ConfigMenu *) _data->UserData)->SetNetworkInput(_data->Buf, NetworkInput::VISION_PORT);
    }
    return 0;
}

// Referee
int ConfigMenu::HandleRefereeIpChange(ImGuiInputTextCallbackData *_data)
{
    if (std::regex_match(_data->Buf, ipRegex))
    {
        ((ConfigMenu *) _data->UserData)->SetNetworkInput(_data->Buf, NetworkInput::REF_IP);
    }
    return 0;
}

int ConfigMenu::HandleRefereePortChange(ImGuiInputTextCallbackData *_data)
{
    if (std::regex_match(_data->Buf, portRegex))
    {
        ((ConfigMenu *) _data->UserData)->SetNetworkInput(_data->Buf, NetworkInput::REF_PORT);
    }
    return 0;
}

void ConfigMenu::SetNetworkInput(std::string _data, NetworkInput _inputType)
{
    switch (_inputType)
    {
    case NetworkInput::VISION_IP:
        if (_data != this->vision_ip_text)
        {
            strcpy(this->vision_ip_text, _data.c_str());
            this->networkNeedsUpdate = NetworkInput::VISION_IP;
            Tyr::Common::setting().updateSetting("network.vision.ip", _data);
            Tyr::Common::Services::saveConfig();
        }
        break;
    case NetworkInput::VISION_PORT:
        if (_data != this->vision_port_text)
        {
            strcpy(this->vision_port_text, _data.c_str());
            this->networkNeedsUpdate = NetworkInput::VISION_PORT;
            Tyr::Common::setting().updateSetting("network.vision.port", std::stoi(_data));
            Tyr::Common::Services::saveConfig();
        }
        break;
    case NetworkInput::REF_IP:
        if (_data != this->referee_ip_text)
        {
            strcpy(this->referee_ip_text, _data.c_str());
            this->networkNeedsUpdate = NetworkInput::REF_IP;
            Tyr::Common::setting().updateSetting("network.referee.ip", _data);
            Tyr::Common::Services::saveConfig();
        }
        break;
    case NetworkInput::REF_PORT:
        if (_data != this->referee_port_text)
        {
            strcpy(this->referee_port_text, _data.c_str());
            this->networkNeedsUpdate = NetworkInput::REF_PORT;
            Tyr::Common::setting().updateSetting("network.referee.port", std::stoi(_data));
            Tyr::Common::Services::saveConfig();
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
    case NetworkInput::VISION_IP:
        return this->vision_ip_text;
        break;
    case NetworkInput::VISION_PORT:
        return this->vision_port_text;
        break;
    default:
        return "";
        break;
    }
}

void ConfigMenu::drawIpPortInput(std::string _name, int _id, char *_ip_text, char *_port_text,
                                 int (*_callback_ip)(ImGuiInputTextCallbackData *),
                                 int (*_callback_port)(ImGuiInputTextCallbackData *))
{
    ImGui::Spacing();
    ImGui::Text(_name.c_str());
    ImGui::Spacing();
    ImGui::PushID(_id);
    ImGui::InputText("Ip", _ip_text, 16, ImGuiInputTextFlags_CallbackAlways, _callback_ip, this);
    ImGui::InputText("Port", _port_text, 6, ImGuiInputTextFlags_CallbackAlways, _callback_port, this);
    ImGui::PopID();
    ImGui::Spacing();
    ImGui::Separator();
}

void ConfigMenu::DrawNetworkTab()
{

    drawIpPortInput("Vision", 0, vision_ip_text, vision_port_text, &ConfigMenu::HandleVisionIpChange,
                    &ConfigMenu::HandleVisionPortChange);
    drawIpPortInput("Referee", 1, referee_ip_text, referee_port_text, &ConfigMenu::HandleRefereeIpChange,
                    &ConfigMenu::HandleRefereePortChange);
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
