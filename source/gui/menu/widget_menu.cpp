#include "widget_menu.h"

namespace Tyr::Gui
{
WidgetMenu::WidgetMenu()
{
#ifdef __APPLE__
    SetGamepadMappings("030000005e040000220b000017050000,Xbox Wireless "
                       "Controller,a:b0,b:b1,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,"
                       "leftshoulder:b6,leftstick:b13,lefttrigger:a5,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,"
                       "righttrigger:a4,rightx:a2,righty:a3,start:b11,x:b3,y:b4,platform:Mac OS X,");
#endif

    m_lstick_texture  = LoadRenderTexture(150., 150.);
    m_rstick_texture  = LoadRenderTexture(150., 150.);
    m_buttons_texture = LoadRenderTexture(385., 200.);

    m_udp        = std::make_unique<Common::UdpServer>();
    m_ref_packet = std::make_unique<Protos::Referee>();
    m_address    = Common::setting().referee_address;
    m_team_info.set_name("Immortals");
    m_team_info.set_score(0);
    m_team_info.set_red_cards(0);
    m_team_info.set_yellow_cards(0);
    m_team_info.set_timeouts(0);
    m_team_info.set_timeout_time(0);
    m_team_info.set_goalkeeper(Common::setting().init_gk_id);
}

void WidgetMenu::drawJoystick()
{
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    const char *controller_choices[] = {"Disable", "Referee", "Robot Control"};
    ImGui::Combo("Mode", &m_controller_mode, controller_choices, IM_ARRAYSIZE(controller_choices));

    int                      axis_count = GetGamepadAxisCount(0);
    std::unique_ptr<float[]> axis       = std::make_unique<float[]>(axis_count);
    std::unique_ptr<float[]> buttons    = std::make_unique<float[]>(18);

    for (auto i = 0; i < axis_count; i++)
    {
        axis[i] = GetGamepadAxisMovement(0, i);
    }

    for (auto i = 0; i < 18; i++)
    {
        buttons[i] = IsGamepadButtonDown(0, i);
    }

    BeginTextureMode(m_lstick_texture);
    ClearBackground(BLACK);
    DrawCircle(75, 75, 75., LIGHTGRAY);
    DrawCircle(axis[0] * 50 + 75, axis[1] * -50 + 75, 15., BLUE);
    EndTextureMode();

    BeginTextureMode(m_rstick_texture);
    ClearBackground(BLACK);
    DrawCircle(75, 75, 75., LIGHTGRAY);
    DrawCircle(axis[2] * 50 + 75, axis[3] * -50 + 75, 15., RED);
    EndTextureMode();

    ImGui::BeginGroup();
    ImGui::Image(&m_lstick_texture.texture, {75, 75});
    ImGui::Text("Left stick");
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::Spacing();
    ImGui::SameLine();
    ImGui::BeginGroup();
    ImGui::Image(&m_rstick_texture.texture, {75, 75});
    ImGui::Text("Right stick");
    ImGui::EndGroup();
    ImGui::ProgressBar(axis[4] / 2. + 0.5, ImVec2(0.0f, 0.0f), "L2");
    ImGui::ProgressBar(axis[5] / 2. + 0.5, ImVec2(0.0f, 0.0f), "R2");

    BeginTextureMode(m_buttons_texture);
    ClearBackground(LIGHTGRAY);
    DrawCircle(270, 70, 14., buttons[8] ? RED : DARKGRAY);
    DrawCircle(310, 110, 14., buttons[5] ? RED : DARKGRAY);
    DrawCircle(350, 70, 14., buttons[6] ? RED : DARKGRAY);
    DrawCircle(310, 30, 14., buttons[7] ? RED : DARKGRAY);

    DrawRectangle(80, 20, 20, 40, buttons[3] ? RED : DARKGRAY);
    DrawRectangle(40, 60, 40, 20, buttons[4] ? RED : DARKGRAY);
    DrawRectangle(80, 80, 20, 40, buttons[1] ? RED : DARKGRAY);
    DrawRectangle(100, 60, 40, 20, buttons[2] ? RED : DARKGRAY);

    DrawRectangle(70, 160, 40, 30, {static_cast<unsigned char>(axis[4] * 127 + 127), 0, 40, 255});
    DrawRectangle(70, 130, 40, 20, buttons[9] ? RED : DARKGRAY);
    DrawRectangle(290, 160, 40, 30, {static_cast<unsigned char>(axis[5] * 127 + 127), 0, 40, 255});
    DrawRectangle(290, 130, 40, 20, buttons[11] ? RED : DARKGRAY);

    DrawRectangle(140, 110, 20, 10, buttons[13] ? RED : DARKGRAY);
    DrawRectangle(170, 110, 20, 10, buttons[15] ? RED : DARKGRAY);

    DrawCircle(35, 130, 27., buttons[16] ? RED : DARKGRAY);
    DrawCircle(230, 30, 27., buttons[17] ? RED : DARKGRAY);

    EndTextureMode();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Image(&m_buttons_texture.texture, {385, 200});
}

void WidgetMenu::drawControllerTab()
{
    ImGui::Spacing();
    ImGui::Spacing();
    if (IsGamepadAvailable(0))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
        ImGui::Text("Controller connected [%s]", GetGamepadName(0));
        ImGui::PopStyleColor();
        drawJoystick();
        if (m_controller_mode == static_cast<int>(ControllerMode::REF))
            switch (GetGamepadButtonPressed())
            {
            case 7:
                refBroadcast(Protos::Referee_Command::Referee_Command_FORCE_START);
                break;
            case 6:
                refBroadcast(Protos::Referee_Command::Referee_Command_STOP);
                break;
            case 8:
                if (Common::setting().our_color == Common::TeamColor::Blue)
                {
                    refBroadcast(Protos::Referee_Command::Referee_Command_DIRECT_FREE_BLUE);
                }
                else
                {
                    refBroadcast(Protos::Referee_Command::Referee_Command_DIRECT_FREE_YELLOW);
                }
                break;
            case 5:
                if (Common::setting().our_color == Common::TeamColor::Blue)
                {
                    refBroadcast(Protos::Referee_Command::Referee_Command_PREPARE_KICKOFF_BLUE);
                }
                else
                {
                    refBroadcast(Protos::Referee_Command::Referee_Command_PREPARE_KICKOFF_YELLOW);
                }
                break;
            case 15:
                refBroadcast(Protos::Referee_Command::Referee_Command_HALT);
                break;
            case 13:
                if (Common::setting().our_color == Common::TeamColor::Blue)
                {
                    refBroadcast(Protos::Referee_Command::Referee_Command_BALL_PLACEMENT_BLUE);
                }
                else
                {
                    refBroadcast(Protos::Referee_Command::Referee_Command_BALL_PLACEMENT_YELLOW);
                }
                break;
            case 11:
                refBroadcast(Protos::Referee_Command::Referee_Command_NORMAL_START);
                break;
            case 9:
                if (Common::setting().our_color == Common::TeamColor::Blue)
                {
                    refBroadcast(Protos::Referee_Command::Referee_Command_PREPARE_PENALTY_BLUE);
                }
                else
                {
                    refBroadcast(Protos::Referee_Command::Referee_Command_PREPARE_PENALTY_YELLOW);
                }
                break;
            default:
                break;
            }
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        ImGui::Text("No Controller connected");
        ImGui::PopStyleColor();
        m_controller_mode = static_cast<int>(ControllerMode::DISABLED);
    }
}

void WidgetMenu::drawTabBar()
{
    if (ImGui::BeginTabBar("Config tabs"))
    {
        if (ImGui::BeginTabItem("Gamepad"))
        {
            drawControllerTab();
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
void WidgetMenu::draw(Common::Vec2 _mouse_pos)
{
    auto main_window_height = GetScreenHeight();
    auto main_window_width  = GetScreenWidth();

    m_mouse_pos = _mouse_pos;
    if (ImGui::IsMouseClicked(0))
    {
        m_clicked_mouse_pos = m_mouse_pos;
    }
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
    ImGui::SetNextWindowPos(ImVec2(main_window_width - 400., 0));
    ImGui::SetNextWindowSize(ImVec2(400., main_window_height * 0.7));
    if (ImGui::Begin("Widgets", nullptr, window_flags))
    {
        drawTabBar();
        ImGui::End();
    }
}

void WidgetMenu::refBroadcast(Protos::Referee_Command _command)
{
    static Protos::Referee_Command last_command = Protos::Referee_Command::Referee_Command_HALT;
    if (_command != last_command)
    {
        if (_command == Protos::Referee_Command::Referee_Command_BALL_PLACEMENT_BLUE ||
            _command == Protos::Referee_Command::Referee_Command_BALL_PLACEMENT_YELLOW)
        {
            auto *designated_position = m_ref_packet->mutable_designated_position();

            designated_position->set_x(m_clicked_mouse_pos.x);
            designated_position->set_y(m_clicked_mouse_pos.y);
        }
        m_ref_packet->set_command(_command);
        m_ref_packet->set_command_timestamp(1);
        m_ref_packet->set_packet_timestamp(1);
        m_ref_packet->set_command_counter(m_command_counter);
        m_ref_packet->set_stage(m_stage);
        m_ref_packet->mutable_blue()->CopyFrom(m_team_info);
        m_ref_packet->mutable_yellow()->CopyFrom(m_team_info);
        m_udp->send(*m_ref_packet, m_address);
        m_ref_packet->Clear();
        m_command_counter++;
        last_command = _command;
    }
}
} // namespace Tyr::Gui