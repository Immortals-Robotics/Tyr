#pragma once

#include "obstacle_new.h"

namespace Tyr::Soccer
{
void clear_map();
bool IsInObstacle(Common::Vec2 p);
bool collisionDetect(Common::Vec2 p1, Common::Vec2 p2);
void AddCircle(float x, float y, float r);
void AddRectangle(float x, float y, float w, float h);
} // namespace Tyr::Soccer