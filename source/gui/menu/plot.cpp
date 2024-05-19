#include "footer_menu.h"

namespace Tyr::Gui
{

template <typename T>
void FooterMenu::pushPlotData(const T &t_entity, const unsigned long &t_time, const bool &t_playback)
{
    if (m_last_ts == t_time)
    {
        return;
    }
    if (std::abs(static_cast<long long>(t_time) - static_cast<long long>(m_last_ts)) > 200)
    {
        m_plot_data.clear();
    }
    auto data = PlotData(t_entity.velocity, t_time);
    if (m_plot_data.size() == m_plot_queue_size)
    {
        m_plot_data.pop_front();
    }
    m_plot_data.push_back(data);
    m_last_ts = t_time;
    if (t_playback && m_plot_data.size() > 1)
    {
        std::sort(m_plot_data.begin(), m_plot_data.end(), [](const PlotData &t_first, const PlotData &t_second)
                  { return t_first.timestamp < t_second.timestamp; });
    }
}

void FooterMenu::drawPlot(const Common::WorldState &t_world, const bool &t_playback)
{
    if (ImGui::Button(m_plot_pin_icon.c_str()))
    {
        m_is_plot_pinned ^= 1;
    }
    ImGui::SameLine();
    const char *item_choices[] = {"Our Robot", "Opp Robot", "Ball", "Custom 1", "Custom 2", "Custom 3"};
    ImGui::SetNextItemWidth(200);
    if (ImGui::Combo("##Item", &m_item, item_choices, IM_ARRAYSIZE(item_choices)))
    {
        m_plot_data.clear();
    }
    ImGui::SameLine();

    if (m_item < 2)
    {
        const char *id_choices[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};
        ImGui::SetNextItemWidth(200);
        if (ImGui::Combo("##m_id", &m_id, id_choices, IM_ARRAYSIZE(id_choices)))
        {
            m_plot_data.clear();
        }
        ImGui::SameLine();
    }

    if (m_item < 3)
    {
        const char *data_choices[] = {"vel", "vel xy"};
        ImGui::SetNextItemWidth(200);
        if (ImGui::Combo("##m_fata", &m_type, data_choices, IM_ARRAYSIZE(data_choices)))
        {
            m_plot_data.clear();
        }
        ImGui::SameLine();
    }

    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 30);
    if (ImGui::Button(m_plot_resize_icon.c_str()))
    {
        if (m_is_plot_maximized)
        {
            m_is_plot_maximized = false;
            m_plot_resize_icon  = "\uf31e";
        }
        else
        {
            m_plot_resize_icon  = "\uf78c";
            m_is_plot_maximized = true;
        }
    }
    ImGui::Separator();

    switch (static_cast<PlotItems>(m_item))
    {
    case PlotItems::OurRobot:
        for (const auto &robot : t_world.own_robot)
        {
            if (m_id == robot.vision_id)
            {
                pushPlotData(robot, t_world.time.milliseconds(), t_playback);
            }
        }
        break;
    case PlotItems::OppRobot:
        for (const auto &robot : t_world.opp_robot)
        {
            if (m_id == robot.vision_id)
            {
                pushPlotData(robot, t_world.time.milliseconds(), t_playback);
            }
        }
        break;
    case PlotItems::Ball:
        pushPlotData(t_world.ball, t_world.time.milliseconds(), t_playback);
        break;
    default:
        break;
    }

    if (ImPlot::BeginPlot("My Plot", ImVec2(-1, ImGui::GetWindowHeight() - 60.)), ImGuiCond_Always)
    {

        std::vector<double> times;
        std::vector<double> x_values;
        std::vector<double> y_values;

        times.reserve(m_plot_data.size());
        x_values.reserve(m_plot_data.size());
        y_values.reserve(m_plot_data.size());

        for (const auto &data : m_plot_data)
        {
            times.push_back(static_cast<double>(data.timestamp - m_plot_data.at(0).timestamp) / 1000.);
            switch (static_cast<PlotType>(m_type))
            {
            case PlotType::Vel:
                x_values.push_back(static_cast<double>(data.velocity.length()) / 1000.);
                break;
            default:
                x_values.push_back(static_cast<double>(data.velocity.x) / 1000.);
                y_values.push_back(static_cast<double>(data.velocity.y) / 1000.);
                break;
            }
        }
        //        ImPlot::Set
        const ImPlotAxisFlags time_flag = ImPlotAxisFlags_None;
        const ImPlotAxisFlags vel_flag  = ImPlotAxisFlags_AutoFit;
        ImPlot::SetupAxes("time (s)", "Vel (m/s)", time_flag, vel_flag);
        if (t_playback)
        {
            ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, 0, INFINITY);
        }
        else
        {
            ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, 0, 30);
        }
        ImPlot::SetupAxisZoomConstraints(ImAxis_X1, 3., INFINITY);

        switch (static_cast<PlotType>(m_type))
        {
        case PlotType::XY:
            ImPlot::PlotLine("Vel x", times.data(), x_values.data(), m_plot_data.size());
            ImPlot::PlotLine("Vel y", times.data(), y_values.data(), m_plot_data.size());
            break;
        case PlotType::Vel:
            ImPlot::PlotLine("Vel", times.data(), x_values.data(), m_plot_data.size());
            break;
        default:
            break;
        }

        ImPlot::EndPlot();
    }
}
} // namespace Tyr::Gui