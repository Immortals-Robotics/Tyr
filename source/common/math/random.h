#pragma once

namespace Tyr::Common
{
class Random
{
private:
    std::random_device                    *rnd_device;
    std::mt19937                          *rnd_engine;
    std::uniform_real_distribution<float> *rnd_dist;

    float min;
    float max;

public:
    Random();
    Random(float _min, float _max);
    ~Random();

    float get() const;
};
} // namespace Tyr::Common
