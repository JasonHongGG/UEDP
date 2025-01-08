#pragma once
#include "../../imgui/imgui.h"

struct ShadowConfig
{
	float shadow_thickness = 40.0f;
	ImVec4 shadow_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	bool shadow_filled = false;
	ImVec4 shape_color = ImVec4(0.9f, 0.6f, 0.3f, 1.0f);
	float shape_rounding = 0.0f;
	ImVec2 shadow_offset = ImVec2(0.0f, 0.0f);
	ImVec4 background_color = ImVec4(0.5f, 0.5f, 0.7f, 1.0f);
	bool wireframe = false;
	bool aa = true;
	int poly_shape_index = 0;
};
inline ShadowConfig ShadowConf;