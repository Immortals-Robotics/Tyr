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
    const std::filesystem::path ps5_texture_dir  = data_dir / "ps5.png";

    m_joystick_texture = LoadRenderTexture(770., 400.);

    m_udp = std::make_unique<Common::UdpServer>();

    m_xbox_texture = LoadTexture(xbox_texture_dir.string().c_str());
    m_ps5_texture  = LoadTexture(ps5_texture_dir.string().c_str());
}

void WidgetMenu::drawJoystick()
{
    const char *controller_choices[] = {"Disable", "Referee", "Robot Control"};

    BeginTextureMode(m_joystick_texture);
    ClearBackground(BLACK);
    Color transparent = {.r = 0, .g = 0, .b = 0, .a = 0};

    if (std::string(GetGamepadName(0)).find("Xbox") != std::string::npos)
    {
        DrawTexturePro(m_xbox_texture, {0, 0, 800, 450}, {385, 200, 800, 450}, {400, 225}, 180., LIGHTGRAY);

        DrawCircle(486, 275, 15., IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT) ? RED : BLACK);
        DrawCircle(522, 310, 15., IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_UP) ? RED : BLACK);
        DrawCircle(558, 275, 15., IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) ? RED : BLACK);
        DrawCircle(522, 239, 15., IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) ? RED : BLACK);

        DrawRectangle(301, 150, 22, 30, IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN) ? RED : BLACK);
        DrawRectangle(272, 177, 30, 22, IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT) ? RED : BLACK);
        DrawRectangle(301, 197, 22, 30, IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP) ? RED : BLACK);
        DrawRectangle(321, 177, 30, 22, IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) ? RED : BLACK);

        DrawCircle(530, 360, 10., IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1) ? RED : BLACK);
        DrawCircle(230, 360, 10., IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1) ? RED : BLACK);
        DrawRectangle(610, 300, 30, GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) * 40 + 42,
                      {static_cast<unsigned char>(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) * 127 + 127), 0,
                       255, 255});
        DrawRectangle(120, 300, 30, GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_TRIGGER) * 40 + 42,
                      {static_cast<unsigned char>(GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_TRIGGER) * 127 + 127), 0,
                       255, 255});

        DrawCircle(422, 275, 10., IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) ? RED : BLACK);
        DrawCircle(337, 275, 10., IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_LEFT) ? RED : BLACK);
        DrawCircle(245 + GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) * 40,
                   273 - GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) * 40, 27.,
                   IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_THUMB) ? RED : GREEN);
        DrawCircle(446 + GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X) * 40,
                   188 - GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y) * 40, 27.,
                   IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_THUMB) ? RED : GREEN);
    }
    else
    {
        DrawTexturePro(m_ps5_texture, {0, 0, 954, 674}, {385, 200, 800, 450}, {400, 225}, 180., WHITE);
        DrawCircle(547, 270, 15., IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT) ? RED : transparent);
        DrawCircle(597, 310, 15., IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_UP) ? RED : transparent);
        DrawCircle(647, 270, 15., IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) ? RED : transparent);
        DrawCircle(597, 230, 15., IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) ? RED : transparent);

        DrawRectangle(162, 290, 20, 20, IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP) ? RED : transparent);
        DrawRectangle(122, 260, 20, 20, IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT) ? RED : transparent);
        DrawRectangle(162, 230, 20, 20, IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN) ? RED : transparent);
        DrawRectangle(202, 260, 20, 20, IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) ? RED : transparent);

        DrawCircle(590, 365, 10., IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1) ? RED : transparent);
        DrawCircle(180, 365, 10., IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1) ? RED : transparent);

        DrawRectangle(695, 300, 30, GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) * 40 + 42,
                      {static_cast<unsigned char>(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) * 127 + 127), 0,
                       255, 255});
        DrawRectangle(40, 300, 30, GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_TRIGGER) * 40 + 42,
                      {static_cast<unsigned char>(GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_TRIGGER) * 127 + 127), 0,
                       255, 255});

        DrawCircle(545, 330, 10., IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) ? RED : transparent);
        DrawCircle(225, 330, 10., IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_LEFT) ? RED : transparent);

        DrawCircle(275 + GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) * 40,
                   195 - GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) * 40, 27.,
                   IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_THUMB) ? RED : GREEN);
        DrawCircle(495 + GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X) * 40,
                   195 - GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y) * 40, 27.,
                   IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_THUMB) ? RED : GREEN);
    }
    EndTextureMode();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Image(&m_joystick_texture.texture, {385, 220});
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Combo("Mode", (int *) &m_controller_mode, controller_choices, IM_ARRAYSIZE(controller_choices));
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
        if (m_controller_mode == ControllerMode::Referee)
            switch (GetGamepadButtonPressed())
            {
            case GAMEPAD_BUTTON_RIGHT_FACE_DOWN:
                refBroadcast(Protos::Ssl::Gc::Referee_Command_FORCE_START);
                break;
            case GAMEPAD_BUTTON_RIGHT_FACE_RIGHT:
                refBroadcast(Protos::Ssl::Gc::Referee_Command_STOP);
                break;
            case GAMEPAD_BUTTON_RIGHT_FACE_LEFT:
                if (Common::setting().our_color == Common::TeamColor::Blue)
                {
                    refBroadcast(Protos::Ssl::Gc::Referee_Command_DIRECT_FREE_BLUE);
                }
                else
                {
                    refBroadcast(Protos::Ssl::Gc::Referee_Command_DIRECT_FREE_YELLOW);
                }
                break;
            case GAMEPAD_BUTTON_RIGHT_FACE_UP:
                if (Common::setting().our_color == Common::TeamColor::Blue)
                {
                    refBroadcast(Protos::Ssl::Gc::Referee_Command_PREPARE_KICKOFF_BLUE);
                }
                else
                {
                    refBroadcast(Protos::Ssl::Gc::Referee_Command_PREPARE_KICKOFF_YELLOW);
                }
                break;
            case GAMEPAD_BUTTON_MIDDLE_RIGHT:
                refBroadcast(Protos::Ssl::Gc::Referee_Command_HALT);
                break;
            case GAMEPAD_BUTTON_MIDDLE_LEFT:
                if (Common::setting().our_color == Common::TeamColor::Blue)
                {
                    refBroadcast(Protos::Ssl::Gc::Referee_Command_BALL_PLACEMENT_BLUE);
                }
                else
                {
                    refBroadcast(Protos::Ssl::Gc::Referee_Command_BALL_PLACEMENT_YELLOW);
                }
                break;
            case GAMEPAD_BUTTON_RIGHT_TRIGGER_1:
                refBroadcast(Protos::Ssl::Gc::Referee_Command_NORMAL_START);
                break;
            case GAMEPAD_BUTTON_LEFT_TRIGGER_1:
                if (Common::setting().our_color == Common::TeamColor::Blue)
                {
                    refBroadcast(Protos::Ssl::Gc::Referee_Command_PREPARE_PENALTY_BLUE);
                }
                else
                {
                    refBroadcast(Protos::Ssl::Gc::Referee_Command_PREPARE_PENALTY_YELLOW);
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
        m_controller_mode = ControllerMode::Disabled;
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
void WidgetMenu::draw(const Common::Vec2 t_mouse_pos)
{
    if (ImGui::IsMouseClicked(0))
    {
        m_clicked_mouse_pos = t_mouse_pos;
    }

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
    ImGui::SetNextWindowPos(ImVec2(GetScreenWidth() - 400., 0));
    ImGui::SetNextWindowSize(ImVec2(400., 400.));
    if (ImGui::Begin("Widgets", nullptr, window_flags))
    {
        drawTabBar();
        ImGui::End();
    }
}

void WidgetMenu::refBroadcast(const Protos::Ssl::Gc::Referee_Command t_command)
{
    if (t_command != m_last_command)
    {
        Protos::Ssl::Gc::Referee ref_packet;

        if (t_command == Protos::Ssl::Gc::Referee_Command_BALL_PLACEMENT_BLUE ||
            t_command == Protos::Ssl::Gc::Referee_Command_BALL_PLACEMENT_YELLOW)
        {
            m_clicked_mouse_pos.fillProto(ref_packet.mutable_designated_position());
        }

        ref_packet.set_command(t_command);
        ref_packet.set_command_timestamp(1);
        ref_packet.set_packet_timestamp(1);
        ref_packet.set_command_counter(m_command_counter);
        ref_packet.set_stage(Protos::Ssl::Gc::Referee_Stage_NORMAL_FIRST_HALF);

        ref_packet.mutable_blue()->set_name("Immortals");
        ref_packet.mutable_blue()->set_score(0);
        ref_packet.mutable_blue()->set_red_cards(0);
        ref_packet.mutable_blue()->set_yellow_cards(0);
        ref_packet.mutable_blue()->set_timeouts(0);
        ref_packet.mutable_blue()->set_timeout_time(0);
        ref_packet.mutable_blue()->set_goalkeeper(Common::setting().init_gk_id);

        ref_packet.mutable_yellow()->set_name("Immortals");
        ref_packet.mutable_yellow()->set_score(0);
        ref_packet.mutable_yellow()->set_red_cards(0);
        ref_packet.mutable_yellow()->set_yellow_cards(0);
        ref_packet.mutable_yellow()->set_timeouts(0);
        ref_packet.mutable_yellow()->set_timeout_time(0);
        ref_packet.mutable_yellow()->set_goalkeeper(Common::setting().init_gk_id);

        m_udp->send(ref_packet, Common::setting().referee_address);

        m_command_counter++;
        m_last_command = t_command;
    }
}
} // namespace Tyr::Gui