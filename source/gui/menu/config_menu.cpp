#include "config_menu.h"

namespace Tyr::Gui
{
static const std::regex ip_regex(
    "(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])");
static const std::regex uint_regex("^[1-9][0-9]*$");
static const std::regex float_regex("^[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)$");

ConfigMenu::ConfigMenu() : m_network_needs_update(InputCallbackType::None)
{
    m_window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoDecoration;
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

    std::to_chars(m_ball_merge_distance_text, &m_ball_merge_distance_text[9],
                  Tyr::Common::setting().ball_merge_distance);
    std::to_chars(m_max_ball_2_frame_dist, &m_max_ball_2_frame_dist[9], Tyr::Common::setting().max_ball_2_frame_dist);
    std::to_chars(m_max_ball_frame_not_seen, &m_max_ball_frame_not_seen[9],
                  Tyr::Common::setting().max_ball_frame_not_seen);
    std::to_chars(m_max_ball_hist, &m_max_ball_hist[9], Tyr::Common::setting().max_ball_hist);
    std::to_chars(m_max_robot_frame_not_seen, &m_max_robot_frame_not_seen[9],
                  Tyr::Common::setting().max_robot_frame_not_seen);
    std::to_chars(m_merge_distance, &m_merge_distance[9], Tyr::Common::setting().merge_distance);

    std::to_chars(m_vision_frame_rate, &m_vision_frame_rate[9], Tyr::Common::setting().vision_frame_rate);
    m_use_camera     = Common::setting().use_camera;
    m_use_kalman_ang = Common::setting().use_kalman_ang;
    m_use_kalman_pos = Common::setting().use_kalman_pos;
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
    _callback.setParams(_callback_type_ip, ip_regex);
    ImGui::InputText("Ip", _ip_text, 16, ImGuiInputTextFlags_CallbackAlways, handleInputChange, &_callback);
    _callback.setParams(_callback_type_port, uint_regex);
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

void ConfigMenu::drawConfigTab()
{
    ImGui::Spacing();
    ImGui::Text("Vision");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::SetNextItemWidth(70);
    if (ImGui::InputText("Ball merge distance", m_ball_merge_distance_text, 10))
    {
        if (std::regex_match(m_ball_merge_distance_text, float_regex))
        {
            Tyr::Common::setting().updateSetting("vision.ball_merge_distance", atof(m_ball_merge_distance_text));
            Tyr::Common::Services::saveConfig();
        }
    }
    ImGui::SetNextItemWidth(70);
    if (ImGui::InputText("Max ball 2 frame dist", m_max_ball_2_frame_dist, 10))
    {
        if (std::regex_match(m_max_ball_2_frame_dist, float_regex))
        {
            Tyr::Common::setting().updateSetting("vision.max_ball_2_frame_dist", atof(m_max_ball_2_frame_dist));
            Tyr::Common::Services::saveConfig();
        }
    }

    ImGui::SetNextItemWidth(70);
    if (ImGui::InputText("Max ball frame not seen", m_max_ball_frame_not_seen, 10))
    {
        if (std::regex_match(m_max_ball_frame_not_seen, uint_regex))
        {
            Tyr::Common::setting().updateSetting("vision.max_ball_frame_not_seen", atoi(m_max_ball_frame_not_seen));
            Tyr::Common::Services::saveConfig();
        }
    }

    ImGui::SetNextItemWidth(70);
    if (ImGui::InputText("Max ball hist", m_max_ball_hist, 10))
    {
        if (std::regex_match(m_max_ball_hist, uint_regex))
        {
            Tyr::Common::setting().updateSetting("vision.max_ball_hist", atoi(m_max_ball_hist));
            Tyr::Common::Services::saveConfig();
        }
    }

    ImGui::SetNextItemWidth(70);
    if (ImGui::InputText("Max robot frame not seen", m_max_robot_frame_not_seen, 10))
    {
        if (std::regex_match(m_max_robot_frame_not_seen, uint_regex))
        {
            Tyr::Common::setting().updateSetting("vision.max_robot_frame_not_seen", atoi(m_max_robot_frame_not_seen));
            Tyr::Common::Services::saveConfig();
        }
    }

    ImGui::SetNextItemWidth(70);
    if (ImGui::InputText("merge distance", m_merge_distance, 10))
    {
        if (std::regex_match(m_merge_distance, float_regex))
        {
            Tyr::Common::setting().updateSetting("vision.merge_distance", atof(m_merge_distance));
            Tyr::Common::Services::saveConfig();
        }
    }

    ImGui::SetNextItemWidth(70);
    if (ImGui::InputText("Vision frame rate", m_vision_frame_rate, 10))
    {
        if (std::regex_match(m_vision_frame_rate, float_regex))
        {
            Tyr::Common::setting().updateSetting("vision.vision_frame_rate", atof(m_vision_frame_rate));
            Tyr::Common::Services::saveConfig();
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    for (auto i = 0; i < max_num_of_cameras; i++)
    {
        if (i % 2 == 1)
        {
            ImGui::SameLine();
        }
        if (ImGui::Checkbox(std::string("Camera " + std::to_string(i)).c_str(), &m_use_camera[i]))
        {
            Tyr::Common::setting().updateSetting("vision.use_camera", m_use_camera);
            Tyr::Common::Services::saveConfig();
        }
    }
    ImGui::Spacing();
    ImGui::Separator();

    if (ImGui::Checkbox("Use kalman Angle", &m_use_kalman_ang))
    {
        Tyr::Common::setting().updateSetting("vision.use_kalman_ang", m_use_kalman_ang);
        Tyr::Common::Services::saveConfig();
    }
    if (ImGui::Checkbox("Use kalman Pos", &m_use_kalman_pos))
    {
        Tyr::Common::setting().updateSetting("vision.use_kalman_pos", m_use_kalman_pos);
        Tyr::Common::Services::saveConfig();
    }

    ImGui::Spacing();
    ImGui::Separator();
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
        if (ImGui::BeginTabItem("Config"))
        {
            drawConfigTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Debug Filter"))
        {
            drawFilterTab();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void ConfigMenu::draw()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    auto main_window_height = GetScreenHeight();
    auto main_window_width = GetScreenWidth();
    if (((main_window_width - 650.) * 0.77) >= main_window_height - 200.)
    {
        main_window_width = (main_window_height - 200.) / 0.77 + 650.;
    }

    ImGui::SetNextWindowSize(ImVec2(250., (main_window_width - 650.) * 0.77));
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

ConfigCallback::ConfigCallback(ConfigMenu *t_menu)
{
    m_menu = t_menu;
}

void ConfigCallback::setParams(const InputCallbackType t_type, const std::regex t_regex)
{
    m_callback_type = t_type;
    m_regex         = t_regex;
}

const std::regex ConfigCallback::getRegex()
{
    return m_regex;
}

void ConfigCallback::executeCallback(std::string t_data)
{
    m_menu->setNetworkInput(t_data, m_callback_type);
}

} // namespace Tyr::Gui
