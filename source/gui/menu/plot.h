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
        Velocity = 0,
        VelocityXY  = 1,
        Angle = 2,
        AngularVelocity = 3,
        Position = 4,
        PosVelY = 5, // used for delay measurement

        Count,
    };

    enum class Item
    {
        OurRobot = 0,
        OppRobot = 1,
        Ball     = 2,
    };

    void pushData(const Common::WorldState &t_state, const bool &t_playback);

    // used for delay measurement
    std::pair<Common::Duration, Common::Vec2> pos(const int t_idx) const
    {
        const Common::WorldState &state = m_data[t_idx];
        Common::Vec2              data{};

        switch (m_item)
        {
        case Item::OurRobot:
            data = state.own_robot[m_id].position;
            break;
        case Item::OppRobot:
            data = state.opp_robot[m_id].position;
            break;
        case Item::Ball:
            data = state.ball.position;
            break;
        }

        const Common::Duration time = state.time - m_data.at(0).time;
        return {time, data};
    }

    std::pair<Common::Duration, Common::Vec2> velocity(const int t_idx) const
    {
        const Common::WorldState &state = m_data[t_idx];
        Common::Vec2              data{};

        switch (m_item)
        {
        case Item::OurRobot:
            data = state.own_robot[m_id].velocity;
            break;
        case Item::OppRobot:
            data = state.opp_robot[m_id].velocity;
            break;
        case Item::Ball:
            data = state.ball.velocity;
            break;
        }

        const Common::Duration time = state.time - m_data.at(0).time;
        return {time, data};
    }

    std::pair<Common::Duration, Common::Angle> angle(const int t_idx) const
    {
        const Common::WorldState &state = m_data[t_idx];
        Common::Angle              data{};

        switch (m_item)
        {
        case Item::OurRobot:
            data = state.own_robot[m_id].angle;
            break;
        case Item::OppRobot:
            data = state.opp_robot[m_id].angle;
            break;
        default:
            break;
        }

        const Common::Duration time = state.time - m_data.at(0).time;
        return {time, data};
    }

    std::pair<Common::Duration, Common::Angle> angularVelocity(const int t_idx) const
    {
        const Common::WorldState &state = m_data[t_idx];
        Common::Angle              data{};

        switch (m_item)
        {
        case Item::OurRobot:
            data = state.own_robot[m_id].angular_velocity;
            break;
        case Item::OppRobot:
            data = state.opp_robot[m_id].angular_velocity;
            break;
        default:
                break;
        }

        const Common::Duration time = state.time - m_data.at(0).time;
        return {time, data};
    }

    static constexpr int kPlotQueueSize = 1000;

    Item m_item = Item::OurRobot;
    int  m_id   = 0;
    Type m_type = Type::Velocity;

    Common::TimePoint m_last_time;

    std::deque<Common::WorldState> m_data;
};
} // namespace Tyr::Gui
