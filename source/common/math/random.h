#pragma once

namespace Immortals::Common
{
class Random
{
public:
    Random() : m_rnd_engine(std::random_device()())
    {}

    float get(const float t_min, const float t_max)
    {
        std::uniform_real_distribution<float> rnd_dist(t_min, t_max);
        return rnd_dist(m_rnd_engine);
    }

    // returns a random number in range [t_min, t_max]
    int get(const int t_min, const int t_max)
    {
        std::uniform_int_distribution<int> rnd_dist(t_min, t_max);
        return rnd_dist(m_rnd_engine);
    }

private:
    std::default_random_engine m_rnd_engine;
};
} // namespace Immortals::Common
