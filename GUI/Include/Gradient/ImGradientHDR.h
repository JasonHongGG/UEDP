#pragma once

#include <array>
#include <stdint.h>
#include "../../../imgui/imgui.h"

const int32_t MarkerMax = 8;

enum class ImGradientHDRMarkerType
{
	Color,
	Alpha,
	Unknown,
};

struct ImGradientHDRState
{

	ImGradientHDRMarkerType selectedMarkerType = ImGradientHDRMarkerType::Unknown;
	int selectedIndex = -1;

	ImGradientHDRMarkerType draggingMarkerType = ImGradientHDRMarkerType::Unknown;
	int draggingIndex = -1;

	void ResetGlobalPara(ImGradientHDRState& state) {
		state.selectedMarkerType = ImGradientHDRMarkerType::Unknown;
		state.selectedIndex = -1;
		state.draggingMarkerType = ImGradientHDRMarkerType::Unknown;
		state.draggingIndex = -1;
	}

	// ============================================
	struct ColorMarker
	{
		float Position;
		std::array<float, 3> Color;
		float Intensity;
	};

	struct AlphaMarker
	{
		float Position;
		float Alpha;
	};

	int ColorCount = 0;
	int AlphaCount = 0;
	std::array<ColorMarker, MarkerMax> Colors;
	std::array<AlphaMarker, MarkerMax> Alphas;

	ColorMarker* GetColorMarker(int32_t index);

	AlphaMarker* GetAlphaMarker(int32_t index);

	bool AddColorMarker(float x, std::array<float, 3> color, float intensity);

	bool AddAlphaMarker(float x, float alpha);

	bool RemoveColorMarker(int32_t index);

	bool RemoveAlphaMarker(int32_t index);

	std::array<float, 4> GetCombinedColor(float x) const;

	std::array<float, 4> GetColorAndIntensity(float x) const;

	float GetAlpha(float x) const;
};

bool ImGradientHDR(int32_t gradientID, ImGradientHDRState& state, ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x - 5/*margin*/ * 2, 20), bool isMarkerShown = true);
