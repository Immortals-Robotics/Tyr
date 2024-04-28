#pragma once

// TODO: remove these and use common Vec2 operations

namespace Tyr::Gui
{
// Additions
const ImVec2 operator+(const ImVec2 &a, const ImVec2 &b);
const ImVec2 operator+=(ImVec2 &a, const ImVec2 &b);
const ImVec4 operator+(const ImVec4 &a, const ImVec4 &b);
const ImVec4 operator+=(ImVec4 &a, const ImVec4 &b);

// Subtractions
const ImVec2 operator-(const ImVec2 &a, const ImVec2 &b);
const ImVec2 operator-=(ImVec2 &a, const ImVec2 &b);
const ImVec4 operator-(const ImVec4 &a, const ImVec4 &b);
const ImVec4 operator-=(ImVec4 &a, const ImVec4 &b);
const ImVec2 operator-(const ImVec2 &a);
const ImVec4 operator-(const ImVec4 &a);

// Multiplications
const ImVec2 operator*(const ImVec2 &a, const ImVec2 &b);
const ImVec2 operator*=(ImVec2 &a, const ImVec2 &b);
const ImVec4 operator*(const ImVec4 &a, const ImVec4 &b);
const ImVec4 operator*=(ImVec4 &a, const ImVec4 &b);
const ImVec2 operator*(const ImVec2 &a, const float &b);
const ImVec2 operator*=(ImVec2 &a, const float &b);
const ImVec4 operator*(const ImVec4 &a, const float &b);
const ImVec4 operator*=(ImVec4 &a, const float &b);

// Divisions
const ImVec2 operator/(const ImVec2 &a, const float &b);
const ImVec2 operator/=(ImVec2 &a, const float &b);
const ImVec4 operator/(const ImVec4 &a, const float &b);
const ImVec4 operator/=(ImVec4 &a, const float &b);
} // namespace Tyr::Gui
