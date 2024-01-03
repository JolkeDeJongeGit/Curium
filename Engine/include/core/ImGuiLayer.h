#pragma once
#include <include/imgui.h>

namespace ImGuiLayer
{
	void Init();
	void UpdateWindow(const float inDt);
	void NewFrame();
	void Render(void* inCommandList);
	void Shutdown();
	void _Init();
	static float uiScale = 1.0f;
};

namespace ImGui
{
	void Tooltip(const char* inTooltip);
	ImVec4 Interpolate(const ImVec4 inColor1, const ImVec4 inColor2, const float inFraction);
	bool ButtonCenteredOnLine(const char* label, float alignment = 0.5f);
}

