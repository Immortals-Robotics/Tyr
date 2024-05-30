#pragma once

namespace Tyr::Gui
{
class PlotMenu
{
public:
     PlotMenu();
    ~PlotMenu();

    void draw(const Common::WorldState &t_world, const bool &t_playback);

    void clear()
    {
        m_data.clear();
    }

private:
    enum class Type
    {
        Vel = 0,
        XY  = 1,
    };

    enum class Item
    {
        OurRobot = 0,
        OppRobot = 1,
        Ball     = 2,
    };

    struct Data
    {
        Data(const Common::Vec2 &t_vec, const Common::TimePoint &t_time) : data(t_vec), time(t_time)
        {}

        Common::Vec2      data;
        Common::TimePoint time;
    };

    static constexpr int kPlotQueueSize = 1000;

    Item m_item = Item::OurRobot;
    int  m_id   = 0;
    Type m_type = Type::Vel;

    Common::TimePoint m_last_time;

    std::deque<Data> m_data;

    void pushData(const Common::Vec2 &t_data, const Common::TimePoint &t_time, const bool &t_playback);
};
} // namespace Tyr::Gui
