#pragma once

#include "duration.h"
#include "time_point.h"

namespace Immortals::Common
{
class Timer
{
public:
    void start()
    {
        m_start  = TimePoint::now();
        m_paused = false;
    }

    void reset()
    {
        start();
        pause();
    }

    void pause()
    {
        if (!m_paused)
        {
            m_time_when_paused = time();
            m_paused           = true;
        }
    }

    void resume()
    {
        if (m_paused)
        {
            m_start  = TimePoint::now() - m_time_when_paused;
            m_paused = false;
        }
    }

    bool paused() const
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

    Duration time() const
    {
        return m_paused ? m_time_when_paused : TimePoint::now() - m_start;
    }

    Duration interval()
    {
        const Duration duration = time();
        start();
        return duration;
    }

    Duration intervalSmooth()
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
} // namespace Immortals::Common
