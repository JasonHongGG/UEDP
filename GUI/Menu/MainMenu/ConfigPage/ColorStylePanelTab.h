#pragma once
#include "../../../../imgui/imgui.h"
#include "../../../MyGuiComponent.h"
#include "../../../Config/FontConfig.h"

namespace ColorStylePanelTab
{
	void Render() 
	{
		ImGuiStyle& style = ImGui::GetStyle();
		static ImGuiStyle ref_saved_style = style;
		ImGuiStyle* ref = &ref_saved_style;			// 為顏色原始的基準點

		static ImGuiTextFilter filter;
		filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

		static ImGuiColorEditFlags alpha_flags = 0;
		if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_None)) { alpha_flags = ImGuiColorEditFlags_None; } ImGui::SameLine();
		if (ImGui::RadioButton("Alpha", alpha_flags == ImGuiColorEditFlags_AlphaPreview)) { alpha_flags = ImGuiColorEditFlags_AlphaPreview; } ImGui::SameLine();
		if (ImGui::RadioButton("Both", alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf)) { alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf; } ImGui::SameLine();
		ImGui::HelpMarker("In the color list:\n""Left-click on color square to open color picker,\n""Right-click to open edit options menu.");

		ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), ImVec2(FLT_MAX, FLT_MAX));
		ImGui::BeginChild("##colors", ImVec2(0, 0), ImGuiChildFlags_Border, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
		{
			ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
			for (int i = 0; i < ImGuiCol_COUNT; i++)
			{
				const char* name = ImGui::GetStyleColorName(i);
				if (!filter.PassFilter(name))
					continue;
				ImGui::PushID(i);

				ImGui::PushFont(Font::IconText);
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0, 0, 0, 0));
				if (ImGui::Button(ICON_FA_QUESTION))
					ImGui::DebugFlashStyleColor((ImGuiCol)i);
				ImGui::SetItemTooltip("Flash given color to identify places where it is used.");
				ImGui::PopStyleColor(5);
				ImGui::PopStyleVar();
				ImGui::PopFont();
				ImGui::SameLine();

				ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
				if (memcmp(&style.Colors[i], &ref->Colors[i], sizeof(ImVec4)) != 0)
				{
					ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Save")) { ref->Colors[i] = style.Colors[i]; }
					ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Revert")) { style.Colors[i] = ref->Colors[i]; }
				}
				ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
				ImGui::TextUnformatted(name);
				ImGui::PopID();
			}
			ImGui::PopItemWidth();
		}
		ImGui::EndChild();
	}
}