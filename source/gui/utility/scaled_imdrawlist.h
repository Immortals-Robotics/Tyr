#pragma once

#include "vector_helper.h"

#define IM_PI 3.14159265358979323846f
#define IM_2PI 6.28318530717958647692f

struct ImDrawListScaled
{
	ImDrawList *drawList;

	ImVec2 offset;
	float scale;
	ImVec2 VerticalFlip;
	ImDrawListScaled()
	{
		drawList = NULL;
		offset = ImVec2(0.f, 0.f);
		scale = 1.f;
		VerticalFlip = ImVec2(1.f, -1.f);
	}

	// Primitives
	inline void AddLine(const ImVec2 &a, const ImVec2 &b, ImU32 col, float thickness = 1.0f) const
	{
		drawList->AddLine(offset + VerticalFlip * a * scale, offset + VerticalFlip * b * scale, col, thickness);
	}

	inline void AddRect(const ImVec2 &a, const ImVec2 &b, ImU32 col, float rounding = 0.0f, int rounding_corners = 0x0F) const
	// a: upper-left, b: lower-right
	{
		drawList->AddRect(offset + VerticalFlip * a * scale, offset + VerticalFlip * b * scale, col, rounding, rounding_corners);
	}

	inline void AddRectFilled(const ImVec2 &a, const ImVec2 &b, ImU32 col, float rounding = 0.0f, int rounding_corners = 0x0F) const
	// a: upper-left, b: lower-right
	{
		drawList->AddRectFilled(offset + VerticalFlip * a * scale, offset + VerticalFlip * b * scale, col, rounding, rounding_corners);
	}

	inline void AddCircle(const ImVec2 &centre, float radius, ImU32 col, int num_segments = 12) const
	{
		drawList->AddCircle(offset + VerticalFlip * centre * scale, radius * scale, col, num_segments);
	}

	inline void AddCircleFilled(const ImVec2 &centre, float radius, ImU32 col, int num_segments = 12) const
	{
		drawList->AddCircleFilled(offset + VerticalFlip * centre * scale, radius * scale, col, num_segments);
	}

	inline void AddText(const ImVec2 &pos, ImU32 col, const char *text_begin, const char *text_end = NULL) const
	{
		drawList->AddText(offset + VerticalFlip * pos * scale, col, text_begin, text_end);
	}

	inline void AddText(const ImFont *font, float font_size, const ImVec2 &pos, ImU32 col, const char *text_begin, const char *text_end = NULL, float wrap_width = 0.0f, const ImVec4 *cpu_fine_clip_rect = NULL)
	{
		drawList->AddText(font, font_size * scale * 10, offset + VerticalFlip * pos * scale, col, text_begin, text_end, wrap_width, cpu_fine_clip_rect);
	}

	// Stateful path API, add points then finish with PathFill() or PathStroke()
	inline void PathClear() const
	{
		drawList->PathClear();
	}
	inline void PathLineTo(const ImVec2 &pos) const
	{
		drawList->PathLineTo(offset + VerticalFlip * pos * scale);
	}
	inline void PathLineToMergeDuplicate(const ImVec2 &pos) const
	{
		drawList->PathLineToMergeDuplicate(offset + VerticalFlip * pos * scale);
	}
	inline void PathFill(ImU32 col) const
	{
		drawList->PathFillConvex(col);
	}
	inline void PathStroke(ImU32 col, bool closed, float thickness = 1.0f) const
	{
		drawList->PathStroke(col, closed, thickness);
	}
	inline void PathArcTo(const ImVec2 &centre, float radius, float a_min, float a_max, int num_segments = 10) const
	{
		drawList->PathArcTo(offset + VerticalFlip * centre * scale, radius * scale, a_min, a_max, num_segments);
	}
	inline void PathArcToFast(const ImVec2 &centre, float radius, int a_min_of_12, int a_max_of_12) const
	{
		drawList->PathArcToFast(offset + VerticalFlip * centre * scale, radius * scale, a_min_of_12, a_max_of_12);
	}
	inline void PathBezierCurveTo(const ImVec2 &p1, const ImVec2 &p2, const ImVec2 &p3, int num_segments = 0) const
	{
		drawList->PathBezierCubicCurveTo(offset + VerticalFlip * p1 * scale, offset + VerticalFlip * p2 * scale, offset + VerticalFlip * p3 * scale, num_segments);
	}
	inline void PathRect(const ImVec2 &rect_min, const ImVec2 &rect_max, float rounding = 0.0f, int rounding_corners = 0x0F) const
	{
		drawList->PathRect(offset + VerticalFlip * rect_min * scale, offset + VerticalFlip * rect_max * scale, rounding, rounding_corners);
	}
};