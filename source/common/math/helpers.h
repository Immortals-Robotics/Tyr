#pragma once

#include <deque>
#include <vector>

namespace Tyr::Common
{
bool almostEqual(float t_a, float t_b);

float sign(float t_f);

float average_deque(std::deque<float> _q);
float variance_deque(std::deque<float> _q);

// This method performs the abc formula (Pythagoras' Theorem) on the given parameters.
std::vector<float> abcFormula(float t_a, float t_b, float t_c);
} // namespace Tyr::Common
