#include "widget_menu.h"

namespace Tyr::Gui
{
WidgetMenu::WidgetMenu()
{
    glfwInit();
    m_lstick_texture  = LoadRenderTexture(150., 150.);
    m_rstick_texture  = LoadRenderTexture(150., 150.);
    m_buttons_texture = LoadRenderTexture(350., 200.);
}

void WidgetMenu::drawControllerTab()
{
    ImGui::Spacing();
    ImGui::Spacing();

    if (glfwJoystickPresent(GLFW_JOYSTICK_1))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
        ImGui::Text("Controller connected [%s]", glfwGetJoystickName(GLFW_JOYSTICK_1));
        ImGui::PopStyleColor();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();
        const char *controller_choices[] = {"Disable", "Referee", "Robot Control"};
        ImGui::Combo("Mode", &m_controller_mode, controller_choices, IM_ARRAYSIZE(controller_choices));
        
        ///RAYLIB
        Common::logDebug("axis left {}", GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X));

        int          js_count;
        const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &js_count);
        const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &js_count);
        
        BeginTextureMode(m_lstick_texture);
        DrawCircle(75, 75, 75., LIGHTGRAY);
        DrawCircle(axes[0] * 50 + 75, axes[1] * -50 + 75, 15., BLUE);
        EndTextureMode();

        BeginTextureMode(m_rstick_texture);
        DrawCircle(75, 75, 75., LIGHTGRAY);
        DrawCircle(axes[2] * 50 + 75, axes[3] * -50 + 75, 15., RED);
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
        ImGui::ProgressBar(axes[4] / 2. + 0.5, ImVec2(0.0f, 0.0f), "L2");
        ImGui::ProgressBar(axes[5] / 2. + 0.5, ImVec2(0.0f, 0.0f), "R2");

        BeginTextureMode(m_buttons_texture);
        ClearBackground(LIGHTGRAY);
      
        DrawCircle(222, 100, 14., DARKGRAY);
        DrawCircle(300, 178, 14., DARKGRAY);
        DrawCircle(378, 100, 14., DARKGRAY);
        DrawCircle(300, 22, 14., DARKGRAY);

        EndTextureMode();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Image(&m_buttons_texture.texture, {350, 200});

        ImGui::Bullet();
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
void WidgetMenu::draw()
{
    auto             main_window_height = GetScreenHeight();
    auto             main_window_width  = GetScreenWidth();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
    ImGui::SetNextWindowPos(ImVec2(main_window_width - 400., 0));
    ImGui::SetNextWindowSize(ImVec2(400., main_window_height / 2.));
    if (ImGui::Begin("Widgets", nullptr, window_flags))
    {
        drawTabBar();
        ImGui::End();
    }
}
} // namespace Tyr::Gui