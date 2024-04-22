#include "vector_helper.h"

// Additions
const ImVec2 operator+(const ImVec2 &a, const ImVec2 &b)
{
	return ImVec2(a.x + b.x, a.y + b.y);
}

const ImVec2 operator+=(ImVec2 &a, const ImVec2 &b)
{
	return a = ImVec2(a.x + b.x, a.y + b.y);
}

const ImVec4 operator+(const ImVec4 &a, const ImVec4 &b)
{
	return ImVec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

const ImVec4 operator+=(ImVec4 &a, const ImVec4 &b)
{
	return a = ImVec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

// Subtractions
const ImVec2 operator-(const ImVec2 &a, const ImVec2 &b)
{
	return ImVec2(a.x - b.x, a.y - b.y);
}

const ImVec2 operator-=(ImVec2 &a, const ImVec2 &b)
{
	return a = ImVec2(a.x - b.x, a.y - b.y);
}

const ImVec4 operator-(const ImVec4 &a, const ImVec4 &b)
{
	return ImVec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

const ImVec4 operator-=(ImVec4 &a, const ImVec4 &b)
{
	return a = ImVec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

const ImVec2 operator-(const ImVec2 &a)
{
	return ImVec2(-a.x, -a.y);
}
const ImVec4 operator-(const ImVec4 &a)
{
	return ImVec4(-a.x, -a.y, -a.z, -a.w);
}

// Multiplications
const ImVec2 operator*(const ImVec2 &a, const ImVec2 &b)
{
	return ImVec2(a.x * b.x, a.y * b.y);
}

const ImVec2 operator*=(ImVec2 &a, const ImVec2 &b)
{
	return a = ImVec2(a.x * b.x, a.y * b.y);
}

const ImVec4 operator*(const ImVec4 &a, const ImVec4 &b)
{
	return ImVec4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

const ImVec4 operator*=(ImVec4 &a, const ImVec4 &b)
{
	return a = ImVec4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

const ImVec2 operator*(const ImVec2 &a, const float &b)
{
	return ImVec2(a.x * b, a.y * b);
}

const ImVec2 operator*=(ImVec2 &a, const float &b)
{
	return a = ImVec2(a.x * b, a.y * b);
}

const ImVec4 operator*(const ImVec4 &a, const float &b)
{
	return ImVec4(a.x * b, a.y * b, a.z * b, a.w * b);
}

const ImVec4 operator*=(ImVec4 &a, const float &b)
{
	return a = ImVec4(a.x * b, a.y * b, a.z * b, a.w * b);
}

// Divisions
const ImVec2 operator/(const ImVec2 &a, const float &b)
{
	return ImVec2(a.x / b, a.y / b);
}

const ImVec2 operator/=(ImVec2 &a, const float &b)
{
	return a = ImVec2(a.x / b, a.y / b);
}

const ImVec4 operator/(const ImVec4 &a, const float &b)
{
	return ImVec4(a.x / b, a.y / b, a.z / b, a.w / b);
}

const ImVec4 operator/=(ImVec4 &a, const float &b)
{
	return a = ImVec4(a.x / b, a.y / b, a.z / b, a.w / b);
}