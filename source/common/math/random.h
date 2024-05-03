#pragma once

namespace Tyr::Common
{
class Random
{
private:
    std::default_random_engine m_rnd_engine;

public:
    Random();

    float get(float t_min = 0.0f, float t_max = 1.0f);
};
} // namespace Tyr::Common
