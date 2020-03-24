#pragma once

#include <imgui.h>
#include <string>
#include <functional>
#include "Vector.h"

extern ImFont* m_pTahomaFont;
extern ImFont* m_pTahomaFontLarge;

class Renderer
{
public:
	void Initialize(ImFont* Font, ImFont* LargeFont);

	void BeginScene();
	void EndScene();

	float RenderText(const std::string& text, const ImVec2& position, float size, uint32_t color, bool center = false, bool Large = false);
	void RenderLine(const ImVec2& from, const ImVec2& to, uint32_t color, float thickness = 1.0f);
	void RenderCircle(const ImVec2& position, float radius, uint32_t color, float thickness = 1.0f, uint32_t segments = 16);
	void RenderCircleFilled(const ImVec2& position, float radius, uint32_t color, uint32_t segments = 16);
	void RenderRect(const ImVec2& from, const ImVec2& to, uint32_t color, float rounding = 0.0f, uint32_t roundingCornersFlags = ImDrawCornerFlags_All, float thickness = 1.0f);
	void RenderRectFilled(const ImVec2& from, const ImVec2& to, uint32_t color, float rounding = 0.0f, uint32_t roundingCornersFlags = ImDrawCornerFlags_All);
};
extern Renderer* g_Renderer;

Vector FromHSB(float hue, float saturation, float brightness);