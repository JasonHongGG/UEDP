#pragma once
#include "../../../imgui/imgui.h"
#include "../../Config/MainMenuConfig.h"
#include "../../Config/FontConfig.h"
#include "../../Config/LoadAndSaveSettingConfig.h"

#include "../../MyGuiComponent.h"
#include "../../../State/GUIState.h"
#include "../../../State/EventHandler.h"

namespace LoadAndSaveSettingPage
{
	void Render(MainMenuCurPage PageID)
	{
		if (PageID != MainMenuCurPage::MenuConfigSL) return;
		
		ImGui::BeginChild("Config R/W", ImVec2(0, 0), ImGuiChildFlags_Border);
		{
			ImGui::PushFont(Font::TitleText);
			ImGui::Text("Config SaveFile");
			ImGui::PopFont();

			ImGui::DummySpace();
			ImGui::InputTextWithHintWithWidth("##SaveNewMenuConfig_FilePathStr", "Save File Path", LoadAndSaveSettingConf.SaveConf.FilePath, ImGui::GetContentRegionAvail().x - 54);
			ImGui::SameLine(0, 4);
			if (ImGui::Button("Load##LoadBtn-ConfigFileLoad", ImVec2(50, 0))) {
				EventHandler::ReadSetting();
			}

			ImGui::DummySpace();
			ImGui::InputTextWithHintWithWidth("##SaveNewMenuConfig_TitleStr", "Title", LoadAndSaveSettingConf.SaveConf.Title, 120);
			ImGui::SameLine(0, 4);
			ImGui::InputTextWithHintWithWidth("##SaveNewMenuConfig_DescriptionStr", "Description", LoadAndSaveSettingConf.SaveConf.Description, (ImGui::GetContentRegionAvail().x - 50/*save btn*/ - 4));
			ImGui::SameLine(0, 4);
			if (ImGui::Button("Save##SaveBtn-ConfigSave", ImVec2(50, 0))) {
				EventHandler::SaveSetting();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10);
			for (int i = 0; i < LoadAndSaveSettingConf.SaveListConf.size(); i++) {
				ImGui::Dummy(ImVec2(0, 10));
				ImGui::BeginChild(("ConfigSave_" + std::to_string(i)).c_str(), ImVec2(0, 200), ImGuiChildFlags_Border);
				{
					ImGui::SetCursorPosX(10);
					ImGui::SetCursorPosY(10);
					ImGui::PushFont(Font::TitleText);
					ImGui::Text(LoadAndSaveSettingConf.SaveListConf[i].Title.c_str());
					ImGui::PopFont();

					ImGui::SetCursorPosX(10);
					ImGui::PushFont(Font::SubTitleText);
					ImGui::Text(LoadAndSaveSettingConf.SaveListConf[i].Description.c_str());
					ImGui::PopFont();

					ImGui::SetCursorPosX(10);
					ImGui::SetCursorPosY(165);
					ImGui::PushFont(Font::SubTitleText);
					ImGui::Text(LoadAndSaveSettingConf.SaveListConf[i].CurTime.c_str());
					ImGui::PopFont();

					ImVec2 WindowSize = ImGui::GetWindowSize();
					ImGui::SetCursorPos(ImVec2(WindowSize.x - 175, 160));
					if (ImGui::Button(("Load##LoadBtn-ConfigSave_" + std::to_string(i)).c_str(), ImVec2(75, 25))) {
						EventHandler::LoadSetting(LoadAndSaveSettingConf.SaveListConf[i].CurTime);
					}
					ImGui::SameLine();
					ImGui::SetCursorPosX(WindowSize.x - 95);
					if (ImGui::Button(("Delete##DeleteBtn-ConfigSave_" + std::to_string(i)).c_str(), ImVec2(75, 25))) {
						if (EventHandler::DeleteSetting(LoadAndSaveSettingConf.SaveListConf[i].CurTime))
							LoadAndSaveSettingConf.SaveListConf.erase(LoadAndSaveSettingConf.SaveListConf.begin() + i);
					}

					ImGui::EndChild();
				}
			}
			ImGui::PopStyleVar();
			ImGui::EndChild();
		}
	}
}