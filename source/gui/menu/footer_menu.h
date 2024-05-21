#pragma once
#include "config_menu.h"

namespace Tyr::Gui
{
class FooterMenu
{
private:
    int           m_filter_level      = 0;
    bool          m_is_plot_maximized = false;
    bool          m_is_plot_pinned    = true;
    std::string   m_plot_resize_icon  = "\uf31e";
    std::string   m_plot_pin_icon     = "\uf08d";
    int           m_item              = 0;
    int           m_id                = 0;
    int           m_type              = 0;
    int           m_plot_queue_size   = 1000;
    ImVec2        m_plot_window_size;
    unsigned long m_last_ts          = 0;
    ImGuiCond     m_window_condition = 0;

    enum class PlotType
    {
        Vel = 0,
        XY  = 1,
    };

    enum class PlotItems
    {
        OurRobot = 0,
        OppRobot = 1,
        Ball     = 2,
        Custom1  = 3,
        Custom2  = 4,
        Custom3  = 5,
    };
    struct PlotData
    {
        PlotData(const Common::Vec2 &t_vec, const unsigned long &t_ts) : velocity(t_vec), timestamp(t_ts)
        {}
        Common::Vec2  velocity;
        unsigned long timestamp;
    };

    std::deque<PlotData> m_plot_data;

    void drawTerminal(const Common::Debug::Wrapper                                    &t_wrapper,
                      const std::unordered_map<std::string, ConfigMenu::FilterNode *> &t_map);

    void drawPlot(const Common::WorldState &t_world, const bool &t_playback);
    template <typename T>
    void pushPlotData(const T &t_entity, const unsigned long &t_time, const bool &t_playback);

public:
    void draw(const Common::Debug::Wrapper                                    &t_wrapper,
              const std::unordered_map<std::string, ConfigMenu::FilterNode *> &t_map, const Common::WorldState &t_world,
              const bool &t_playback);
    void clearPlot()
    {
        m_plot_data.clear();
    }
     FooterMenu();
    ~FooterMenu();
};
} // namespace Tyr::Gui
