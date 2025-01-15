#pragma once
#include "../../../../../imgui/imgui.h"
#include "../../../../Config/GradientHDRConfig.h"
#include "../../../../../State/GUIState.h"

namespace GradientEditor
{
	void Render()
	{
		bool& state = ConfigPageComponentState.OpenGradientEditor;

		ImGui::SetNextWindowSize({ 620,300 }, ImGuiCond_FirstUseEver);
		if (state and ImGui::Begin("Gradient HDR", &state))
		{
			static bool init = true;
			static bool isMarkerShown = true;
			static int stateID = 123456789; // need unique

			if (init) {
				GradientState.AddColorMarker(0.0f, { 1.0f, 1.0f, 1.0f }, 1.0f);
				GradientState.AddColorMarker(1.0f, { 1.0f, 1.0f, 1.0f }, 1.0f);
				GradientState.AddAlphaMarker(0.0f, 1.0f);
				GradientState.AddAlphaMarker(1.0f, 1.0f);
				init = false;
			}

			ImGradientHDR(stateID, GradientState);

			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Gradient");
			}

			if (GradientState.selectedMarkerType == ImGradientHDRMarkerType::Color)
			{
				auto selectedColorMarker = GradientState.GetColorMarker(GradientState.selectedIndex);
				if (selectedColorMarker != nullptr)
				{
					ImGui::ColorEdit3("Color", selectedColorMarker->Color.data(), ImGuiColorEditFlags_Float);
					ImGui::DragFloat("Intensity", &selectedColorMarker->Intensity, 0.1f, 0.0f, 100.0f, "%f", 1.0f);
				}
			}

			if (GradientState.selectedMarkerType == ImGradientHDRMarkerType::Alpha)
			{
				auto selectedAlphaMarker = GradientState.GetAlphaMarker(GradientState.selectedIndex);
				if (selectedAlphaMarker != nullptr)
				{
					ImGui::DragFloat("Alpha", &selectedAlphaMarker->Alpha, 0.1f, 0.0f, 1.0f, "%f", 1.0f);
				}
			}

			if (GradientState.selectedMarkerType != ImGradientHDRMarkerType::Unknown)
			{
				if (ImGui::Button("Delete"))
				{
					if (GradientState.selectedMarkerType == ImGradientHDRMarkerType::Color or
						GradientState.selectedMarkerType == ImGradientHDRMarkerType::Alpha)
					{
						GradientState.RemoveColorMarker(GradientState.selectedIndex);
						GradientState.ResetGlobalPara(GradientState);
					}
				}
			}

			ImGui::End();
		}
	}
}