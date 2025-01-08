#pragma once
#include <string>
#include "../../../../Font/IconsFontAwesome6.h"
#include "../../../Config/StyleConfig.h"
#include "../../../Config/FontConfig.h"

#include "../../../../State/GUIState.h"

namespace ComponentPanelTab
{
	void Render()
	{
		auto BtnComponent = [](bool& IsOpen, std::string Name, ImVec2 BtnSize) -> bool {
			bool oriState = IsOpen;
			if (oriState) Style::ActivateBtnStyleSwitch(true);
			if (ImGui::Button((ICON_FA_TV_ALT"##" + Name).c_str(), BtnSize))
				IsOpen = !IsOpen;
			if (oriState) Style::ActivateBtnStyleSwitch(false);
			return true;
			};

		// Btn
		ImGui::PushFont(Font::BigIconText);
		Style::DefaultBtnStyleSwitch(true);
		static ImVec2 BtnSize = ImVec2(65, 60);
		BtnComponent(ConfigPageComponentStateMgr.OpenShadowEditor, "ShadowEditor", BtnSize);
		ImGui::SameLine();
		BtnComponent(ConfigPageComponentStateMgr.OpenToggleEditor, "ToggleEditor", BtnSize);
		ImGui::SameLine();
		BtnComponent(ConfigPageComponentStateMgr.OpenGradientEditor, "GradientEditor", BtnSize);
		Style::DefaultBtnStyleSwitch(false);
		ImGui::PopFont();
	}
}