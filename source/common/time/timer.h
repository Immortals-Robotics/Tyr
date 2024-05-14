#pragma once

namespace Tyr::Common
{
class Timer
{
public:
    inline void start()
    {
        m_start = std::chrono::high_resolution_clock::now();
    }

    inline float time()
    {
        const auto now       = std::chrono::high_resolution_clock::now();
        const auto time_span = std::chrono::duration_cast<std::chrono::duration<float>>(now - m_start);
        return time_span.count();
    }

    inline float interval()
    {
        const float t = time();
        start();
        return t;
    }

    inline float intervalSmooth()
    {
        static constexpr float kAlpha = 0.1f;

        m_interval = (kAlpha * interval()) + (1.0f - kAlpha) * m_interval;
        return m_interval;
    }

private:
    std::chrono::high_resolution_clock::time_point m_start;

    float m_interval = 0.0f;
};
} // namespace Tyr::Common
