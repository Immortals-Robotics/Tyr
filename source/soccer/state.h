#pragma once

namespace Tyr::Soccer
{
class Robot;
class State
{
public:
    static const Common::WorldState &world()
    {
        return *m_world;
    }

    static const Common::Referee::State &ref()
    {
        return *m_ref;
    }

    static int side()
    {
        return *m_side;
    }

    static const Robot *robot()
    {
        return m_robots;
    }

private:
    friend class Ai;

    static inline const Common::WorldState     *m_world;
    static inline const Common::Referee::State *m_ref;

    static inline const int   *m_side;
    static inline const Robot *m_robots;
};

} // namespace Tyr::Soccer
