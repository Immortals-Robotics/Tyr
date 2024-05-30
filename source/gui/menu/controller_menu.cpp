#include "controller_menu.h"

namespace Tyr::Gui
{
ControllerMenu::ControllerMenu()
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

    m_referee_color = Common::config().common.our_color;

    m_render_target = LoadRenderTexture(770., 400.);

    m_udp = std::make_unique<Common::UdpServer>();

    m_xbox_texture = LoadTexture(xbox_texture_dir.string().c_str());
    m_ps5_texture  = LoadTexture(ps5_texture_dir.string().c_str());
}

void ControllerMenu::renderController()
{
    BeginTextureMode(m_render_target);
    ClearBackground(BLANK);

    if (std::string(GetGamepadName(0)).find("Xbox") != std::string::npos)
    {
        DrawTexturePro(m_xbox_texture, {0, 0, 800, 450}, {385, 200, 800, 450}, {400, 225}, 180., LIGHTGRAY);

        DrawCircle(486, 275, 15., IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT) ? RED : BLANK);
        DrawCircle(522, 310, 15., IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_UP) ? RED : BLANK);
        DrawCircle(558, 275, 15., IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) ? RED : BLANK);
        DrawCircle(522, 239, 15., IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) ? RED : BLANK);

        DrawRectangle(301, 150, 22, 30, IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN) ? RED : BLANK);
        DrawRectangle(272, 177, 30, 22, IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT) ? RED : BLANK);
        DrawRectangle(301, 197, 22, 30, IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP) ? RED : BLANK);
        DrawRectangle(321, 177, 30, 22, IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) ? RED : BLANK);

        DrawCircle(530, 360, 10., IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1) ? RED : BLANK);
        DrawCircle(230, 360, 10., IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1) ? RED : BLANK);
        DrawRectangle(610, 300, 30, GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) * 40 + 42,
                      {static_cast<unsigned char>(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) * 127 + 127), 0,
                       255, 255});
        DrawRectangle(120, 300, 30, GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_TRIGGER) * 40 + 42,
                      {static_cast<unsigned char>(GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_TRIGGER) * 127 + 127), 0,
                       255, 255});

        DrawCircle(422, 275, 10., IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) ? RED : BLANK);
        DrawCircle(337, 275, 10., IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_LEFT) ? RED : BLANK);
        DrawCircle(245 + GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) * 30,
                   273 - GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) * 30, 27.,
                   IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_THUMB) ? RED : GREEN);
        DrawCircle(446 + GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X) * 30,
                   188 - GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y) * 30, 27.,
                   IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_THUMB) ? RED : GREEN);
    }
    else
    {
        DrawTexturePro(m_ps5_texture, {0, 0, 954, 674}, {385, 200, 800, 450}, {400, 225}, 180., WHITE);
        DrawCircle(547, 270, 15., IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT) ? RED : BLANK);
        DrawCircle(597, 310, 15., IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_UP) ? RED : BLANK);
        DrawCircle(647, 270, 15., IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) ? RED : BLANK);
        DrawCircle(597, 230, 15., IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) ? RED : BLANK);

        DrawRectangle(162, 290, 20, 20, IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP) ? RED : BLANK);
        DrawRectangle(122, 260, 20, 20, IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT) ? RED : BLANK);
        DrawRectangle(162, 230, 20, 20, IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN) ? RED : BLANK);
        DrawRectangle(202, 260, 20, 20, IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) ? RED : BLANK);

        DrawCircle(590, 365, 10., IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1) ? RED : BLANK);
        DrawCircle(180, 365, 10., IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1) ? RED : BLANK);

        DrawRectangle(695, 300, 30, GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) * 40 + 42,
                      {static_cast<unsigned char>(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) * 127 + 127), 0,
                       255, 255});
        DrawRectangle(40, 300, 30, GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_TRIGGER) * 40 + 42,
                      {static_cast<unsigned char>(GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_TRIGGER) * 127 + 127), 0,
                       255, 255});

        DrawCircle(545, 330, 10., IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) ? RED : BLANK);
        DrawCircle(225, 330, 10., IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_LEFT) ? RED : BLANK);

        DrawCircle(275 + GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) * 30,
                   195 - GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) * 30, 27.,
                   IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_THUMB) ? RED : GREEN);
        DrawCircle(495 + GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X) * 30,
                   195 - GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y) * 30, 27.,
                   IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_THUMB) ? RED : GREEN);
    }
    EndTextureMode();
}

void ControllerMenu::draw()
{
    ImGui::Spacing();
    ImGui::Spacing();
    if (IsGamepadAvailable(0))
    {
        ImGui::TextColored(static_cast<ImVec4>(Common::Color::green()), "Controller connected [%s]", GetGamepadName(0));

        renderController();

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();

        const float width  = ImGui::GetContentRegionAvail().x;
        const float height = (width / m_render_target.texture.width) * m_render_target.texture.height;
        rlImGuiImageSize(&m_render_target.texture, width, height);

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();

        const char *controller_choices[] = {"Disable", "Referee", "Robot Control"};
        ImGui::Combo("Mode", reinterpret_cast<int *>(&m_controller_mode), controller_choices,
                     IM_ARRAYSIZE(controller_choices));

        if (m_controller_mode == Mode::Referee)
        {
            ImGui::SeparatorText("Referee");
            ImGui::PushID("ControllerReferee");

            static constexpr std::array kEnums = {"Blue", "Yellow"};
            static_assert(static_cast<int>(Common::TeamColor::Blue) == 0);
            static_assert(static_cast<int>(Common::TeamColor::Yellow) == 1);

            ImGui::Combo("Color", reinterpret_cast<int *>(&m_referee_color), kEnums.data(), kEnums.size());

            refereeUpdate();

            ImGui::PopID();
        }
    }
    else
    {
        ImGui::TextColored(static_cast<ImVec4>(Common::Color::red()), "No Controller connected");
        m_controller_mode = Mode::Disabled;
    }
}

std::optional<Protos::Ssl::Gc::Referee_Command> ControllerMenu::refereeCommand() const
{
    switch (GetGamepadButtonPressed())
    {
    case GAMEPAD_BUTTON_RIGHT_FACE_DOWN:
        return Protos::Ssl::Gc::Referee_Command_FORCE_START;
    case GAMEPAD_BUTTON_RIGHT_FACE_RIGHT:
        return Protos::Ssl::Gc::Referee_Command_STOP;
    case GAMEPAD_BUTTON_RIGHT_FACE_LEFT:
        return m_referee_color == Common::TeamColor::Blue ? Protos::Ssl::Gc::Referee_Command_DIRECT_FREE_BLUE
                                                          : Protos::Ssl::Gc::Referee_Command_DIRECT_FREE_YELLOW;
    case GAMEPAD_BUTTON_RIGHT_FACE_UP:
        return m_referee_color == Common::TeamColor::Blue ? Protos::Ssl::Gc::Referee_Command_PREPARE_KICKOFF_BLUE
                                                          : Protos::Ssl::Gc::Referee_Command_PREPARE_KICKOFF_YELLOW;
    case GAMEPAD_BUTTON_MIDDLE_RIGHT:
        return Protos::Ssl::Gc::Referee_Command_HALT;
    case GAMEPAD_BUTTON_MIDDLE_LEFT:
        return m_referee_color == Common::TeamColor::Blue ? Protos::Ssl::Gc::Referee_Command_BALL_PLACEMENT_BLUE
                                                          : Protos::Ssl::Gc::Referee_Command_BALL_PLACEMENT_YELLOW;
    case GAMEPAD_BUTTON_RIGHT_TRIGGER_1:
        return Protos::Ssl::Gc::Referee_Command_NORMAL_START;
    case GAMEPAD_BUTTON_LEFT_TRIGGER_1:
        return m_referee_color == Common::TeamColor::Blue ? Protos::Ssl::Gc::Referee_Command_PREPARE_PENALTY_BLUE
                                                          : Protos::Ssl::Gc::Referee_Command_PREPARE_PENALTY_YELLOW;
    default:
        return {};
    }
}

void ControllerMenu::refereeUpdate()
{
    const auto command = refereeCommand();

    if (!command.has_value())
    {
        return;
    }

    Protos::Ssl::Gc::Referee ref_packet{};

    if (command == Protos::Ssl::Gc::Referee_Command_BALL_PLACEMENT_BLUE ||
        command == Protos::Ssl::Gc::Referee_Command_BALL_PLACEMENT_YELLOW)
    {
        m_clicked_mouse_pos.fillProto(ref_packet.mutable_designated_position());
    }

    const Common::TimePoint now = Common::TimePoint::now();

    ref_packet.set_command(*command);
    ref_packet.set_command_timestamp(now.microseconds());
    ref_packet.set_packet_timestamp(now.microseconds());
    ref_packet.set_command_counter(m_command_counter);
    ref_packet.set_stage(Protos::Ssl::Gc::Referee_Stage_NORMAL_FIRST_HALF);

    ref_packet.mutable_blue()->set_name("Immortals");
    ref_packet.mutable_blue()->set_score(0);
    ref_packet.mutable_blue()->set_red_cards(0);
    ref_packet.mutable_blue()->set_yellow_cards(0);
    ref_packet.mutable_blue()->set_timeouts(0);
    ref_packet.mutable_blue()->set_timeout_time(0);
    ref_packet.mutable_blue()->set_goalkeeper(Common::config().soccer.init_gk_id);

    ref_packet.mutable_yellow()->set_name("Immortals");
    ref_packet.mutable_yellow()->set_score(0);
    ref_packet.mutable_yellow()->set_red_cards(0);
    ref_packet.mutable_yellow()->set_yellow_cards(0);
    ref_packet.mutable_yellow()->set_timeouts(0);
    ref_packet.mutable_yellow()->set_timeout_time(0);
    ref_packet.mutable_yellow()->set_goalkeeper(Common::config().soccer.init_gk_id);

    m_udp->send(ref_packet, Common::config().network.referee_address);

    m_command_counter++;
}
} // namespace Tyr::Gui
