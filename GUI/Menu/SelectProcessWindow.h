#pragma once
#include "../../imgui/imgui.h"
#include "../Config/SelectProcessWindowConfig.h"
#include "../../State/GUIState.h"
#include "../../System/Process.h"
#include "../DockingManager.h"
#include "../GUIComponent/GUIComponent.h"

namespace SelectProcessWindow
{
	void Render()
	{
		if (!SelectProcessWindowState.OpenSelectProcessWindow) return;

		ImGui::PushFont(Font::NormalText);
		ImGui::SetNextWindowSize({ 400,450 }, ImGuiCond_Once);
		ImGui::SetNextWindowPos({ (ProcMgr.WindowMgr.UserWindowWidth - 400.f) / 2, (ProcMgr.WindowMgr.UserWindowHeight - 530.f) / 2 }, ImGuiCond_FirstUseEver);
		ImGui::Begin("Select Process", &SelectProcessWindowState.OpenSelectProcessWindow, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | DockingMgr.GetDockingDisableFlag());
		{
			ImGui::PushID("Select Process");

			ImGui::PushFont(Font::ChineseText);
			MyListBox("##Select Process List", SelectProcessWindowConf.WindowTitleDisplaySelectIndex, SelectProcessWindowConf.WindowTitleList, ImVec2(-FLT_MIN, 400), SelectProcessWindowConf.WindowTitleFilterStr)
				.SetFont(Font::ChineseText)
				.Show();
			ImGui::PopFont();

			ImGui::DummySpace();
			ImGui::InputTextWithHintWithWidth("##SerchWindowName", "Serch Window Name", SelectProcessWindowConf.WindowTitleFilterStr, -FLT_MIN - 200);
			ImGui::SameLine(0, 4);

			if (ImGui::Button("Confirm", ImVec2((ImGui::GetContentRegionAvail().x - 4) / 2, 25))) {
				SelectProcessWindowConf.WindowTitleSelectIndex = SelectProcessWindowConf.WindowTitleDisplaySelectIndex;
				SelectProcessWindowState.OpenSelectProcessWindow = false;
			}
			ImGui::SameLine(0, 4);

			if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvail().x, 25))) {
				SelectProcessWindowState.OpenSelectProcessWindow = false;
			}
			ImGui::PopID();
		}
		ImGui::End();
		ImGui::PopFont();
	}
}