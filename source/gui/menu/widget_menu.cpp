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
    const std::filesystem::path data_dir(DATA_DIR);
    const std::filesystem::path xbox_texture_dir = data_dir / "xbox.png";
    const std::filesystem::path ps5_texture_dir = data_dir / "ps5.png";

    m_joystick_texture = LoadRenderTexture(770., 400.);

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

    m_xbox_texture = LoadTexture(xbox_texture_dir.string().c_str());
    m_ps5_texture = LoadTexture(ps5_texture_dir.string().c_str());
}

void WidgetMenu::drawJoystick()
{
    const char              *controller_choices[] = {"Disable", "Referee", "Robot Control"};
    int                      axis_count           = GetGamepadAxisCount(0);
    std::unique_ptr<float[]> axis                 = std::make_unique<float[]>(axis_count);
    std::unique_ptr<float[]> buttons              = std::make_unique<float[]>(18);

    for (auto i = 0; i < axis_count; i++)
    {
        axis[i] = GetGamepadAxisMovement(0, i);
    }

    for (auto i = 0; i < 18; i++)
    {
        buttons[i] = IsGamepadButtonDown(0, i);
    }

    BeginTextureMode(m_joystick_texture);
    ClearBackground(BLACK);
    DrawTexturePro(m_xbox_texture, {0, 0, 800, 450}, {385, 200, 800, 450}, {400, 225}, 180., LIGHTGRAY);
    DrawCircle(486, 275, 15., buttons[8] ? RED : BLACK);
    DrawCircle(522, 310, 15., buttons[5] ? RED : BLACK);
    DrawCircle(558, 275, 15., buttons[6] ? RED : BLACK);
    DrawCircle(522, 239, 15., buttons[7] ? RED : BLACK);

    DrawRectangle(301, 150, 22, 30, buttons[3] ? RED : BLACK);
    DrawRectangle(272, 177, 30, 22, buttons[4] ? RED : BLACK);
    DrawRectangle(301, 197, 22, 30, buttons[1] ? RED : BLACK);
    DrawRectangle(321, 177, 30, 22, buttons[2] ? RED : BLACK);

    DrawCircle(530, 360, 10., buttons[11] ? RED : BLACK);
    DrawCircle(230, 360, 10., buttons[9] ? RED : BLACK);
    DrawRectangle(610, 300, 30, axis[5] * 40 + 42, {static_cast<unsigned char>(axis[5] * 127 + 127), 0, 255, 255});
    DrawRectangle(120, 300, 30, axis[4] * 40 + 42, {static_cast<unsigned char>(axis[4] * 127 + 127), 0, 255, 255});

    DrawCircle(422, 275, 10., buttons[15] ? RED : BLACK);
    DrawCircle(337, 275, 10., buttons[13] ? RED : BLACK);
    DrawCircle(245 + axis[0] * 40, 273 - axis[1] * 40, 27., buttons[16] ? RED : GREEN);
    DrawCircle(446 + axis[2] * 40, 188 - axis[3] * 40, 27., buttons[17] ? RED : GREEN);

    EndTextureMode();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Image(&m_joystick_texture.texture, {385, 220});
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Combo("Mode", &m_controller_mode, controller_choices, IM_ARRAYSIZE(controller_choices));
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
    ImGui::SetNextWindowSize(ImVec2(400., 400.));
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