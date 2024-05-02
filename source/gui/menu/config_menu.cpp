#include "config_menu.h"

namespace Tyr::Gui
{
static const std::regex ipRegex(
    "(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])");
static const std::regex portRegex("^[1-9][0-9]*$");

ConfigMenu::ConfigMenu() : m_width(300.), m_network_needs_update(InputCallbackType::None)
{
    m_window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
    strcpy(m_vision_ip_text, Tyr::Common::setting().vision_address.ip.c_str());
    strcpy(m_vision_port_text, std::to_string(Tyr::Common::setting().vision_address.port).c_str());

    strcpy(m_referee_ip_text, Tyr::Common::setting().referee_address.ip.c_str());
    strcpy(m_referee_port_text, std::to_string(Tyr::Common::setting().referee_address.port).c_str());

    strcpy(m_sender_ip_text, Tyr::Common::setting().sender_address.ip.c_str());
    strcpy(m_sender_port_text, std::to_string(Tyr::Common::setting().sender_address.port).c_str());

    strcpy(m_strategy_ip_text, Tyr::Common::setting().strategy_address.ip.c_str());
    strcpy(m_strategy_port_text, std::to_string(Tyr::Common::setting().strategy_address.port).c_str());

    strcpy(m_tracker_ip_text, Tyr::Common::setting().tracker_address.ip.c_str());
    strcpy(m_tracker_port_text, std::to_string(Tyr::Common::setting().tracker_address.port).c_str());

    strcpy(m_grsim_ip_text, Tyr::Common::setting().grsim_address.ip.c_str());
    strcpy(m_grsim_port_text, std::to_string(Tyr::Common::setting().grsim_address.port).c_str());
}

// Vision

int ConfigMenu::handleInputChange(ImGuiInputTextCallbackData *_data)
{
    ConfigCallback *callback = ((ConfigCallback *) _data->UserData);

    if (std::regex_match(_data->Buf, callback->getRegex()))
    {
        callback->executeCallback(_data->Buf);
    }
    return 0;
}

void ConfigMenu::setNetworkInput(std::string _data, InputCallbackType _inputType)
{
    try
    {
        if (_data != this->m_type_input_text_map.at(_inputType))
        {
            strcpy(this->m_type_input_text_map.at(_inputType), _data.c_str());
            this->m_network_needs_update = _inputType;
            if (_inputType == InputCallbackType::VISION_PORT || _inputType == InputCallbackType::SENDER_PORT ||
                _inputType == InputCallbackType::STRATEGY_PORT || _inputType == InputCallbackType::REF_PORT ||
                _inputType == InputCallbackType::GRSIM_PORT || _inputType == InputCallbackType::TRACKER_PORT)
            {
                Tyr::Common::setting().updateSetting(config_input_map.at(_inputType), std::stoi(_data));
            }
            else
            {
                Tyr::Common::setting().updateSetting(config_input_map.at(_inputType), _data);
            }
            Tyr::Common::Services::saveConfig();
        }
    }
    catch (const std::out_of_range &e)
    {
        Tyr::Common::logError("Error: Input Key not found in the map");
    }
}

std::string ConfigMenu::getNetworkParam(InputCallbackType _inputType)
{
    try
    {
        return this->m_type_input_text_map.at(_inputType);
    }
    catch (const std::out_of_range &e)
    {
        Tyr::Common::logError("Error: Input Key not found in the map");
        return "";
    }
}

void ConfigMenu::drawIpPortInput(const std::string _name, const int _id, char *_ip_text, char *_port_text,
                                 ConfigCallback &_callback, const InputCallbackType _callback_type_ip,
                                 const InputCallbackType _callback_type_port)
{
    ImGui::Spacing();
    ImGui::Text("%s", _name.c_str());
    ImGui::Spacing();
    ImGui::PushID(_id);
    _callback.setParams(_callback_type_ip, ipRegex);
    ImGui::InputText("Ip", _ip_text, 16, ImGuiInputTextFlags_CallbackAlways, handleInputChange, &_callback);
    _callback.setParams(_callback_type_port, portRegex);
    ImGui::InputText("Port", _port_text, 6, ImGuiInputTextFlags_CallbackAlways, handleInputChange, &_callback);
    ImGui::PopID();
    ImGui::Spacing();
    ImGui::Separator();
}

void ConfigMenu::drawNetworkTab()
{
    static ConfigCallback callback(this);

    drawIpPortInput("Vision", 0, m_vision_ip_text, m_vision_port_text, callback, InputCallbackType::VISION_IP,
                    InputCallbackType::VISION_PORT);
    drawIpPortInput("Referee", 1, m_referee_ip_text, m_referee_port_text, callback, InputCallbackType::REF_IP,
                    InputCallbackType::REF_PORT);
    drawIpPortInput("Sender", 2, m_sender_ip_text, m_sender_port_text, callback, InputCallbackType::SENDER_IP,
                    InputCallbackType::SENDER_PORT);
    drawIpPortInput("Strategy", 3, m_strategy_ip_text, m_strategy_port_text, callback, InputCallbackType::STRATEGY_IP,
                    InputCallbackType::STRATEGY_PORT);
    drawIpPortInput("Tracker", 4, m_tracker_ip_text, m_tracker_port_text, callback, InputCallbackType::TRACKER_IP,
                    InputCallbackType::TRACKER_PORT);
    drawIpPortInput("Grsim", 5, m_grsim_ip_text, m_grsim_port_text, callback, InputCallbackType::GRSIM_IP,
                    InputCallbackType::GRSIM_PORT);
}

void ConfigMenu::drawTabBar()
{
    if (ImGui::BeginTabBar("Config tabs"))
    {
        if (ImGui::BeginTabItem("Network"))
        {
            drawNetworkTab();
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

void ConfigMenu::draw()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(m_width, 700), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Config", nullptr, m_window_flags))
    {
        drawTabBar();
        ImGui::End();
    }
}

InputCallbackType ConfigMenu::isNetworkDataUpdated()
{
    return this->m_network_needs_update;
}

void ConfigMenu::updateNetworkData()
{
    this->m_network_needs_update = InputCallbackType::None;
}

ConfigCallback::ConfigCallback(ConfigMenu *_menu)
{
    m_menu = _menu;
}

void ConfigCallback::setParams(const InputCallbackType _callback_type, const std::regex _regex)
{
    m_callback_type = _callback_type;
    m_regex         = _regex;
}

const std::regex ConfigCallback::getRegex()
{
    return m_regex;
}

void ConfigCallback::executeCallback(std::string _data)
{
    m_menu->setNetworkInput(_data, m_callback_type);
}
} // namespace Tyr::Gui
