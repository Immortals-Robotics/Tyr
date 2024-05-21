#pragma once

#include "duration.h"
#include "time_point.h"

namespace Tyr::Common
{
class Timer
{
public:
    inline void start()
    {
        m_start  = TimePoint::now();
        m_paused = false;
    }

    inline void reset()
    {
        start();
        pause();
    }

    inline void pause()
    {
        if (!m_paused)
        {
            m_time_when_paused = time();
            m_paused           = true;
        }
    }

    inline void resume()
    {
        if (m_paused)
        {
            m_start  = TimePoint::now() - m_time_when_paused;
            m_paused = false;
        }
    }

    inline bool paused() const
    {
        return m_paused;
    }

    void setTime(const Duration t_time)
    {
        if (m_paused)
            m_time_when_paused = t_time;
        else
            m_start += time() - t_time;
    }

    inline Duration time() const
    {
        return m_paused ? m_time_when_paused : TimePoint::now() - m_start;
    }

    inline Duration interval()
    {
        const Duration duration = time();
        start();
        return duration;
    }

    inline Duration intervalSmooth()
    {
        static constexpr double kAlpha = 0.1f;

        m_interval = (kAlpha * interval().seconds()) + (1.0f - kAlpha) * m_interval;
        return Duration::fromSeconds(m_interval);
    }

private:
    TimePoint m_start;

    bool     m_paused = true;
    Duration m_time_when_paused;

    double m_interval = 0.0f;
};
} // namespace Tyr::Common
