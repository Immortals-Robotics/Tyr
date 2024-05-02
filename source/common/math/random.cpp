#include "random.h"

namespace Tyr::Common
{
Random::Random() : m_rnd_engine(std::random_device()())
{}

float Random::get(float t_min, float t_max)
{
    std::uniform_real_distribution<float> rnd_dist(t_min, t_max);
    return rnd_dist(m_rnd_engine);
}
} // namespace Tyr::Common
