#pragma once
#include "../../../imgui/imgui.h"
#include "../../Config/MainMenuConfig.h"

#include "ConfigPage/ColorStylePanelTab.h"
#include "ConfigPage/ComponentPanelTab.h"
#include "ConfigPage/ParamterPanelTab.h"


namespace ConfigPage
{
	void Render(MainMenuCurPage PageID)
	{
		if (PageID != MainMenuCurPage::MainConfig) return;
		
		ImGui::BeginChild("ConfigPage", ImVec2(0, 0), ImGuiChildFlags_Border);
		{
			if (ImGui::BeginTabBar("PackageObjectTabBar", ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_AutoSelectNewTabs))
			{
				for (int n = 0; n < MainMenuConf.ConfigPageTabList.size(); n++) {
					if (MainMenuConf.ConfigPageTabList[n].isOpen and ImGui::BeginTabItem(MainMenuConf.ConfigPageTabList[n].Name.c_str(), &MainMenuConf.ConfigPageTabList[n].isOpen, ImGuiTabItemFlags_None))
					{
						if (MainMenuConf.ConfigPageTabList[n].Type == MainConfigTabType::ColorStylePanel)
							ColorStylePanelTab::Render();
						else if (MainMenuConf.ConfigPageTabList[n].Type == MainConfigTabType::ParamterPanel)
							ParamterPanelTab::Render();
						else if (MainMenuConf.ConfigPageTabList[n].Type == MainConfigTabType::ComponentPanel)
							ComponentPanelTab::Render();
						ImGui::EndTabItem();
					}
				}
			}
			ImGui::EndTabBar();
		}
		ImGui::EndChild();
	}
}