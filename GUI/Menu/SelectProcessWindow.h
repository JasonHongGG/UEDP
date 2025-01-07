#pragma once
#include "../../imgui/imgui.h"
#include "../Config/SelectProcessWindowConfig.h"
#include "../../State/GUIState.h"
#include "../../System/Process.h"

namespace SelectProcessWindow
{
	void Render()
	{
		if (!SelectProcessWindowState.OpenSelectProcessWindow) return;

		ImGui::PushFont(Font::NormalText);
		ImGui::SetNextWindowSize({ 400,530 }, ImGuiCond_Once);
		ImGui::SetNextWindowPos({ (ProcMgr.WindowMgr.UserWindowWidth - 400.f) / 2, (ProcMgr.WindowMgr.UserWindowHeight - 530.f) / 2 }, ImGuiCond_FirstUseEver);
		ImGui::Begin("Select Process", &SelectProcessWindowState.OpenSelectProcessWindow, ImGuiWindowFlags_NoResize | DockingMgr.GetDockingDisableFlag());
		{
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::InputTextWithHintWithWidth("##SerchWindowName", "Serch Window Name", SelectProcessWindowConf.WindowTitleFilterStr);
			ImGui::PopItemWidth();

			ImGui::DummySpace();
			ImGui::PushID("Select Process");

			ImGui::PushFont(Font::ChineseText);
			MyListBox("##Select Process List", SelectProcessWindowConf.WindowTitleDisplaySelectIndex, SelectProcessWindowConf.WindowTitleList, ImVec2(-FLT_MIN, 400), SelectProcessWindowConf.WindowTitleFilterStr)
				.SetFont(Font::ChineseText)
				.Show();
			ImGui::PopFont();

			if (ImGui::Button("Confirm", ImVec2((ImGui::GetContentRegionAvail().x - 4) / 2, 50))) {
				SelectProcessWindowConf.WindowTitleSelectIndex = SelectProcessWindowConf.WindowTitleDisplaySelectIndex;
				SelectProcessWindowState.OpenSelectProcessWindow = false;
			}
			ImGui::SameLine(0, 4);

			if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvail().x, 50))) {
				SelectProcessWindowState.OpenSelectProcessWindow = false;
			}
			ImGui::PopID();
		}
		ImGui::End();
		ImGui::PopFont();
	}
}