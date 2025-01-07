#pragma once
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_dx11.h"
#include "../../imgui/imgui_impl_win32.h"

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
}