#pragma once
#include "../../imgui/imgui.h"

#include "ColorConfig.h"

namespace Style
{
	static void DefaultBtnStyleSwitch(bool activate)
	{
		if (activate) {
			ImGui::PushStyleColor(ImGuiCol_Button, Color::Black);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Color::Black);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, Color::Black);
			ImGui::PushStyleColor(ImGuiCol_Border, Color::LightBlue);
			ImGui::PushStyleColor(ImGuiCol_Text, Color::LightBlue);
		}
		else {
			ImGui::PopStyleColor(5);
		}
	}

	static void ActivateBtnStyleSwitch(bool activate)
	{
		if (activate) {
			ImGui::PushStyleColor(ImGuiCol_Button, Color::LightBlue);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Color::LightBlue);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, Color::LightBlue);
			ImGui::PushStyleColor(ImGuiCol_Text, Color::White);
		}
		else {
			ImGui::PopStyleColor(4);
		}
	}

	static void TransparentButtonStyleSwitch(bool IsApply, ImVec4 Color = { 255, 255, 255, 255 })
	{
		if (IsApply) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_Text, Color);
		}
		else ImGui::PopStyleColor(6);
	}

	static void TabStyleSwitch(bool IsApply)
	{
		if (IsApply) {
			ImGui::PushStyleColor(ImGuiCol_Tab, Color::TabColor);
		}
		else ImGui::PopStyleColor(1);
	}

	static void FramePaddingSwitch(bool IsApply, float Height = 0)
	{
		if (IsApply) ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, Height));
		else        ImGui::PopStyleVar();
	}
}