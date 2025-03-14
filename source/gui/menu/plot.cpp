#include "plot.h"

namespace Tyr::Gui
{
PlotMenu::PlotMenu()
{
    ImPlot::CreateContext();
}

PlotMenu::~PlotMenu()
{
    ImPlot::DestroyContext();
}

void PlotMenu::pushData(const Common::WorldState &t_state, const bool &t_playback)
{
    if (m_last_time == t_state.time)
    {
        return;
    }

    if (std::abs((t_state.time - m_last_time).seconds()) > 0.2f)
    {
        m_data.clear();
    }

    if (m_data.size() >= kPlotQueueSize)
        m_data.pop_front();
    m_data.emplace_back(t_state);

    m_last_time = t_state.time;

    if (t_playback && m_data.size() > 1)
    {
        std::sort(m_data.begin(), m_data.end(),
                  [](const Common::WorldState &t_first, const Common::WorldState &t_second)
                  { return t_first.time < t_second.time; });
    }
}

void PlotMenu::draw(const Common::WorldState &t_world, const bool &t_playback)
{
    pushData(t_world, t_playback);

    if (ImGui::BeginTable("plot-setting", 3, ImGuiTableFlags_BordersInnerV))
    {
        const char *item_choices[] = {"Our Robot", "Opp Robot", "Ball"};

        ImGui::TableNextColumn();
        ImGui::Combo("Item", reinterpret_cast<int *>(&m_item), item_choices, IM_ARRAYSIZE(item_choices));

        if (m_item == Item::OurRobot || m_item == Item::OppRobot)
        {
            const char *id_choices[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

            ImGui::TableNextColumn();
            ImGui::Combo("ID", &m_id, id_choices, IM_ARRAYSIZE(id_choices));
        }

        const char *data_choices[(int) Type::Count];
        data_choices[(int) Type::Velocity]        = "Velocity";
        data_choices[(int) Type::VelocityXY]      = "VelocityXY";
        data_choices[(int) Type::Angle]           = "Angle";
        data_choices[(int) Type::AngularVelocity] = "AngularVelocity";
        data_choices[(int) Type::Position]        = "Position";
        data_choices[(int) Type::PosVelY]         = "PosVelY";

        ImGui::TableNextColumn();
        ImGui::Combo("Data", reinterpret_cast<int *>(&m_type), data_choices, IM_ARRAYSIZE(data_choices));

        ImGui::EndTable();
    }

    ImGui::Separator();

    if (ImPlot::BeginPlot("Plot", ImVec2(-1, ImGui::GetWindowHeight() - 60.), ImPlotFlags_NoTitle))
    {
        auto getter_vel_x = [](const int t_idx, void *t_user_data)
        {
            const PlotMenu &menu = *static_cast<PlotMenu *>(t_user_data);

            const auto [time, data] = menu.velocity(t_idx);
            return ImPlotPoint(time.seconds(), data.x);
        };

        auto getter_vel_y = [](const int t_idx, void *t_user_data)
        {
            const PlotMenu &menu = *static_cast<PlotMenu *>(t_user_data);

            const auto [time, data] = menu.velocity(t_idx);
            return ImPlotPoint(time.seconds(), data.y);
        };

        auto getter_pos_x = [](const int t_idx, void *t_user_data)
        {
            const PlotMenu &menu = *static_cast<PlotMenu *>(t_user_data);

            const auto [time, data] = menu.pos(t_idx);
            return ImPlotPoint(time.seconds(), data.x);
        };

        auto getter_pos_y = [](const int t_idx, void *t_user_data)
        {
            const PlotMenu &menu = *static_cast<PlotMenu *>(t_user_data);

            const auto [time, data] = menu.pos(t_idx);
            return ImPlotPoint(time.seconds(), data.y);
        };

        auto getter_len = [](const int t_idx, void *t_user_data)
        {
            const PlotMenu &menu = *static_cast<PlotMenu *>(t_user_data);

            const auto [time, data] = menu.velocity(t_idx);
            return ImPlotPoint(time.seconds(), data.length());
        };

        auto getter_angle = [](const int t_idx, void *t_user_data)
        {
            const PlotMenu &menu = *static_cast<PlotMenu *>(t_user_data);

            const auto [time, data] = menu.angle(t_idx);
            return ImPlotPoint(time.seconds(), data.deg());
        };

        auto getter_angular_velocity = [](const int t_idx, void *t_user_data)
        {
            const PlotMenu &menu = *static_cast<PlotMenu *>(t_user_data);

            const auto [time, data] = menu.angularVelocity(t_idx);
            return ImPlotPoint(time.seconds(), data.deg());
        };

        switch (m_type)
        {
        case Type::Velocity:
        case Type::VelocityXY:
            ImPlot::SetupAxes("s", "mm/s", ImPlotAxisFlags_None, ImPlotAxisFlags_AutoFit);
            break;
        case Type::Angle:
            ImPlot::SetupAxes("s", "deg", ImPlotAxisFlags_None, ImPlotAxisFlags_AutoFit);
            break;
        case Type::AngularVelocity:
            ImPlot::SetupAxes("s", "deg/s", ImPlotAxisFlags_None, ImPlotAxisFlags_AutoFit);
            break;
        case Type::Position:
            ImPlot::SetupAxes("s", "mm", ImPlotAxisFlags_None, ImPlotAxisFlags_AutoFit);
            break;
        case Type::PosVelY:
            ImPlot::SetupAxes("s", "mm | mm/s", ImPlotAxisFlags_None, ImPlotAxisFlags_AutoFit);
            break;
        default:
            break;
        }

        if (t_playback)
        {
            ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, 0, INFINITY);
        }
        else
        {
            ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, 0, 30);
        }
        ImPlot::SetupAxisZoomConstraints(ImAxis_X1, 3., INFINITY);

        switch (m_type)
        {
        case Type::Velocity:
            ImPlot::PlotLineG("Velocity", getter_len, this, m_data.size());
            break;
        case Type::VelocityXY:
            ImPlot::PlotLineG("Velocity x", getter_vel_x, this, m_data.size());
            ImPlot::PlotLineG("Velocity y", getter_vel_y, this, m_data.size());
            break;
        case Type::Angle:
            ImPlot::PlotLineG("Angle", getter_angle, this, m_data.size());
            break;
        case Type::AngularVelocity:
            ImPlot::PlotLineG("Angular Velocity", getter_angular_velocity, this, m_data.size());
            break;
        case Type::Position:
            ImPlot::PlotLineG("Position x", getter_pos_x, this, m_data.size());
            ImPlot::PlotLineG("Position y", getter_pos_y, this, m_data.size());
            break;
        case Type::PosVelY:
            ImPlot::PlotLineG("Pos Y", getter_pos_y, this, m_data.size());
            ImPlot::PlotLineG("Velocity y", getter_vel_y, this, m_data.size());
            break;
        default:
            break;
        }
        ImPlot::EndPlot();
    }
}
} // namespace Tyr::Gui
