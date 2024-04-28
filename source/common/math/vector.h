#pragma once

#include <linalg.h>

namespace Tyr::Common
{
struct Angle;

using vec2 = linalg::aliases::float2;
using vec3 = linalg::aliases::float3;

using namespace linalg;

vec2 point_on_connecting_line(vec2 FirstPoint, vec2 SecondPoint, float distance);
vec2 circle_around_point(vec2 point, Angle angle, float radius);
} // namespace Tyr::Common
