#pragma once
#include <string>
#include "../../../imgui/imgui.h"

#include "../../Config/TimerConfig.h"
#include "../../Config/MainMenuConfig.h"
#include "../../Config/ColorConfig.h"

#include "../../../State/GlobalState.h"
#include "../../../State/GUIState.h"
#include "../../../State/EventHandler.h"

#include "../../MyGuiComponent.h"
#include "../../../System/Process.h"
#include "../../../Background/Storage/StorageManager.h"


namespace DumperConsole
{
	bool IsAllProcessCompleted()
	{
		if (
			(MainMenuState.Prepareing == ProcessState::Idle or MainMenuState.Prepareing == ProcessState::Completed) and
			(MainMenuState.AutoConfig == ProcessState::Idle or MainMenuState.AutoConfig == ProcessState::Completed) and
			(MainMenuState.GetUEVersion == ProcessState::Idle or MainMenuState.GetUEVersion == ProcessState::Completed) and
			(MainMenuState.GetFNamePool == ProcessState::Idle or MainMenuState.GetFNamePool == ProcessState::Completed) and
			(MainMenuState.GetGUObjectArray == ProcessState::Idle or MainMenuState.GetGUObjectArray == ProcessState::Completed) and
			(MainMenuState.ParseFNamePool == ProcessState::Idle or MainMenuState.ParseFNamePool == ProcessState::Completed) and
			(MainMenuState.ParseGUObjectArray == ProcessState::Idle or MainMenuState.ParseGUObjectArray == ProcessState::Completed)
			)
			return true;
		else
			return false;
	}

	void DumperConsoleStateBtn(const char* ID, const ImVec4& Color, std::string OptionName, ProcessState* OpenSwitch, int Counter) {
		ImGuiContext& g = *GImGui;
		ImGui::PushItemFlag(ImGuiItemFlags_NoTabStop, true);
		if (ImGui::ColorButton(ID, Color, ImGuiColorEditFlags_NoTooltip)) {
			*(OpenSwitch) = ProcessState::Idle;	//��l���A(�Ǧ�)
			MainMenuConf.IsDumperOptionActivate[OptionName] = !MainMenuConf.IsDumperOptionActivate[OptionName];
		}
		ImGui::PopItemFlag();

		// ����L�{�����ʵe
		if (*(OpenSwitch) == ProcessState::Processing or *(OpenSwitch) == ProcessState::Retry)
		{
			//ImRect r = g.LastItemData.Rect;
			ImVec2 center = g.LastItemData.Rect.GetCenter();
			float radius = ImFloor(ImMin(g.LastItemData.Rect.GetWidth(), g.LastItemData.Rect.GetHeight()) * 0.40f);
			float t = (float)(ImGui::GetTime() * 20.0f);
			ImVec2 off(ImCos(t) * radius, ImSin(t) * radius);
			ImGui::GetWindowDrawList()->AddLine(center - off, center + off, ImGui::GetColorU32(ImGuiCol_Text), 1.5f);
			//ImGui::RenderText(r.Min + style.FramePadding + ImVec2(0, 0), &"|\0/\0-\0\\"[(((ImGui::GetFrameCount() / 5) & 3) << 1)], NULL);
		}
		else if (Counter >= 0)
		{
			ImVec2 center = g.LastItemData.Rect.GetCenter();
			std::string buf("%d", Counter);
			ImGui::GetWindowDrawList()->AddText(center - ImGui::CalcTextSize(buf.c_str()) * 0.5f, ImGui::GetColorU32(ImGuiCol_Text), buf.c_str());
		}
	}

	bool DumperConsoleExecutionFilter(std::string OptionName)
	{
		if (MainMenuConf.DumperGlobalExecution == ProcessState::Start) {
			std::string FilterMode = MainMenuConf.DumperGlobalExecutionMode[MainMenuConf.DumperGlobalExecutionMode_SelectedIndex];
			if (FilterMode == "All")
				return true;
			else if (FilterMode == "Activate") {
				if (MainMenuConf.IsDumperOptionActivate[OptionName])
					return true;
			}
			else if (FilterMode == "Deactivate") {
				if (!MainMenuConf.IsDumperOptionActivate[OptionName])
					return true;
			}
		}
		return false;
	}

	bool DumperConsoleTableFilter(std::string Category, std::string OptionName)
	{
		std::string FilterMode = MainMenuConf.DumperGlobalExecutionFilterMode[MainMenuConf.DumperGlobalExecutionFilterMode_SelectedIndex];
		std::string FilterStr = Utils.rStringToLower(std::string(MainMenuConf.DumperOptionFilterStr));		//�����j�p�g�����

		if (FilterMode == "All") {
			if (Utils.rStringToLower(OptionName).find(FilterStr) != std::string::npos || Utils.rStringToLower(Category).find(FilterStr) != std::string::npos)
				return true;
		}
		else if (FilterMode == "OpenName") {
			if (Utils.rStringToLower(OptionName).find(FilterStr) != std::string::npos)
				return true;
		}
		else if (FilterMode == "Category") {
			if (Utils.rStringToLower(Category).find(FilterStr) != std::string::npos)
				return true;
		}
		else if (FilterMode == "Activate") {
			if (MainMenuConf.IsDumperOptionActivate[OptionName] &&
				(Utils.rStringToLower(OptionName).find(FilterStr) != std::string::npos || Utils.rStringToLower(Category).find(FilterStr) != std::string::npos))
				return true;
		}
		else if (FilterMode == "Deactivate") {
			if (!MainMenuConf.IsDumperOptionActivate[OptionName] &&
				(Utils.rStringToLower(OptionName).find(FilterStr) != std::string::npos || Utils.rStringToLower(Category).find(FilterStr) != std::string::npos))
				return true;
		}

		return false;
	}

	void DumperConsoleTableItem(ProcessState* OpenSwitch, std::string Category, std::string OptionName, bool FilterCondition)
	{
		// �p�G�O�L�o�ؼдN������^
		if (!FilterCondition)
			return;

		// ���s�b�N���s�W
		if (MainMenuConf.IsDumperOptionActivate.find(OptionName) == MainMenuConf.IsDumperOptionActivate.end())
			MainMenuConf.IsDumperOptionActivate[OptionName] = false;

		// ����
		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		ImVec4 StatusColor = (*(OpenSwitch) == ProcessState::Completed) ? Color::LightGreen : MainMenuConf.IsDumperOptionActivate[OptionName] ? Color::Orange : Color::Gray;
		DumperConsoleStateBtn(("Run##" + OptionName + "- ColorBtn").c_str(), StatusColor, OptionName, OpenSwitch, -1);
		ImGui::SameLine(0, 0);
		if (ImGui::Button(("Run##" + OptionName + "- Btn").c_str()) or (DumperConsoleExecutionFilter(OptionName))) {	// Btn �Q�I�� or (Blobal Btn �Q�I���B�ŦXFilter������)
			if (ProcessInfo::hProcess) {
				*(OpenSwitch) = ProcessState::Start;
				printf("[Pressed Button] %s\n", OptionName.c_str());
			}
			else {
				EventHandler::NotifyEvent(NotificationConfig::NotiyType::Warning, "No target program attached", "Please select a target program before running");
			}
		}
		ImGui::TableNextColumn();
		ImGui::Text(Category.c_str());
		ImGui::TableNextColumn();
		ImGui::Text(OptionName.c_str());
	}

	void Render()
	{
		// Global Run
		{
			if (ImGui::Button("Run")) {
				if (!ProcessInfo::hProcess) EventHandler::NotifyEvent(NotificationConfig::NotiyType::Warning, "No target program attached", "Please select a target program before running");

				MainMenuConf.DumperGlobalExecution = ProcessState::Start;
				Utils.TimerSwitch(true, &TimerConf.TimeObjMap["DumperTimer"].TimerActivate, TimerConf.TimeObjMap["DumperTimer"].StartTime, TimerConf.TimeObjMap["DumperTimer"].EndTime);
			}

			ImGui::SameLine();
			ImGui::PushItemWidth(100.f);
			MyComboList("##DumperGlobalExecutionMode", MainMenuConf.DumperGlobalExecutionMode_SelectedIndex, MainMenuConf.DumperGlobalExecutionMode).Show();
			ImGui::PopItemWidth();

			ImGui::SameLine();
			ImGui::PushItemWidth(-FLT_MIN - 100);
			ImGui::InputTextWithHint("##SerchDumperFunctionOption", "Serch Dumper Option", MainMenuConf.DumperOptionFilterStr, IM_ARRAYSIZE(MainMenuConf.DumperOptionFilterStr));
			ImGui::PopItemWidth();

			ImGui::SameLine();
			ImGui::PushItemWidth(100.f);
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 100.f);
			MyComboList("##DumperGlobalExecutionFilterMode", MainMenuConf.DumperGlobalExecutionFilterMode_SelectedIndex, MainMenuConf.DumperGlobalExecutionFilterMode).Show();
			ImGui::PopItemWidth();
		}



		// Table Option
		{
			static ImGuiTableFlags TableFlags =
				ImGuiTableFlags_BordersV |          //�̥~�髫�����u
				ImGuiTableFlags_BordersOuterH |     //�̥~��������u
				ImGuiTableFlags_Resizable |
				ImGuiTableFlags_RowBg |
				ImGuiTableFlags_Hideable |      //�i���� column
				ImGuiTableFlags_ScrollX |       //X �b�i�u��
				ImGuiTableFlags_ScrollY |       //Y �b�i�u��
				ImGuiTableFlags_NoBordersInBody |
				ImGuiTableFlags_SizingFixedFit;

			if (ImGui::BeginTable("DumperFunctionOptionTable", 3, TableFlags, ImVec2(0, ImGui::GetContentRegionAvail().y > (35 * 4 + 20) ? ImGui::GetContentRegionAvail().y - (35 * 4) : 20)))           //colum �վ�ɳo��n�վ�
			{
				// Row and Cloumn Fix Setting 
				ImGui::TableSetupScrollFreeze(0, 1);    //�T�w��ܶ}�Y�Ĥ@ column�Brow(�Ӷ���)

				// Header Column Item
				ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_NoHide);
				ImGui::TableSetupColumn("Category");
				ImGui::TableSetupColumn("Option Name", 0, 200.f);
				ImGui::TableHeadersRow();

				// Table Items
				DumperConsoleTableItem(&MainMenuState.ShowBaseAddress, "Info", "ShowBaseAddress", DumperConsoleTableFilter("Info", "ShowBaseAddress"));
				DumperConsoleTableItem(&MainMenuState.Prepareing, "Auto", "Prepareing", DumperConsoleTableFilter("Auto", "Prepareing"));
				DumperConsoleTableItem(&MainMenuState.AutoConfig, "Auto", "AutoConfig", DumperConsoleTableFilter("Auto", "AutoConfig"));
				DumperConsoleTableItem(&MainMenuState.GetUEVersion, "Info", "GetUEVersion", DumperConsoleTableFilter("Info", "GetUEVersion"));
				DumperConsoleTableItem(&MainMenuState.GetFNamePool, "Info", "GetFNamePool", DumperConsoleTableFilter("Info", "GetFNamePool"));
				DumperConsoleTableItem(&MainMenuState.GetGUObjectArray, "Info", "GetGUObjectArray", DumperConsoleTableFilter("Info", "GetGUObjectArray"));
				DumperConsoleTableItem(&MainMenuState.GetGWorld, "Info", "GetGWorld", DumperConsoleTableFilter("Info", "GetGWorld"));
				DumperConsoleTableItem(&MainMenuState.ParseFNamePool, "Info", "ParseFNamePool", DumperConsoleTableFilter("Info", "ParseFNamePool"));
				DumperConsoleTableItem(&MainMenuState.ParseGUObjectArray, "Info", "ParseGUObjectArray", DumperConsoleTableFilter("Info", "ParseGUObjectArray"));

				// ProcessState Tracker
				if (MainMenuConf.DumperGlobalExecution == ProcessState::Start) MainMenuConf.DumperGlobalExecution = ProcessState::Processing;
				if (MainMenuConf.DumperGlobalExecution == ProcessState::Processing and IsAllProcessCompleted()) {
					MainMenuConf.DumperGlobalExecution = ProcessState::Completed;
					Utils.TimerSwitch(false, &TimerConf.TimeObjMap["DumperTimer"].TimerActivate, TimerConf.TimeObjMap["DumperTimer"].StartTime, TimerConf.TimeObjMap["DumperTimer"].EndTime);
				}

				ImGui::EndTable();
			}
		}

		// Progress Bar
		{
			const ImU32 col = ImGui::GetColorU32(Color::ProgressBarColor);
			const ImU32 bg = ImGui::GetColorU32(Color::ProgressBarBGColor);

			// ParseFNamePool Progress
			if (ProgressBarState.FNamePoolEvent == ProcessState::Processing or ProgressBarState.FNamePoolEvent == ProcessState::Completed) {
				ImGui::DummySpace();
				if (ProgressBarState.FNamePoolEvent == ProcessState::Processing) {
					ImGui::Spinner("##ParseFNamePool_Spinner", 10, 6, col);
					ImGui::SameLine();
				}

				std::string BarStr = std::to_string(StorageMgr.FNamePoolParseProgressBarValue.Get()) + "/" + std::to_string(StorageMgr.FNamePoolParseProgressBarTotalValue.Get());
				float BarValue = static_cast<float>(StorageMgr.FNamePoolParseProgressBarValue.Get()) / static_cast<float>(StorageMgr.FNamePoolParseProgressBarTotalValue.Get());
				if (ProgressBarState.FNamePoolEvent == ProcessState::Completed) { BarStr = "[ Completed ]"; BarValue = 1.0f; }
				ImGui::BufferingBar("##ParseFNamePool_BufferBar", BarValue, ImVec2(400, 30), bg, col, BarStr.c_str());
			}

			// ParseGUObjectArray Progress
			if (ProgressBarState.GUObjectArrayEvent == ProcessState::Processing or ProgressBarState.GUObjectArrayEvent == ProcessState::Completed) {
				{
					ImGui::DummySpace();
					if (ProgressBarState.GUObjectArrayEvent == ProcessState::Processing) {
						ImGui::Spinner("##ParseGUObjectArray_Spinner", 10, 6, col);
						ImGui::SameLine();
					}

					std::string BarStr = "[ " + std::to_string(StorageMgr.GUObjectArrayParseIndex.Get()) + " ] " + std::to_string(StorageMgr.GUObjectArrayParseProgressBarValue.Get()) + "/" + std::to_string(StorageMgr.GUObjectArrayParseProgressBarTotalValue.Get());
					float BarValue = static_cast<float>(StorageMgr.GUObjectArrayParseProgressBarValue.Get()) / static_cast<float>(StorageMgr.GUObjectArrayParseProgressBarTotalValue.Get());
					if (ProgressBarState.GUObjectArrayEvent == ProcessState::Completed) { BarStr = "[ Completed ]"; BarValue = 1.0f; }
					ImGui::BufferingBar("##ParseGUObjectArray_BufferBar", BarValue, ImVec2(400, 30), bg, col, BarStr.c_str());
				}
				{
					ImGui::DummySpace();
					if (MainMenuConf.GUObjectArrayTotalObjectProgressBarTotalValue - 10 <= StorageMgr.GUObjectArrayTotalObjectCounter.Get()) MainMenuConf.GUObjectArrayTotalObjectProgressBarTotalValue *= 2;
					std::string BarStr = "[ Total Object Counter ] " + std::to_string(StorageMgr.GUObjectArrayTotalObjectCounter.Get()) + "/" + std::to_string(MainMenuConf.GUObjectArrayTotalObjectProgressBarTotalValue);
					float BarValue = static_cast<float>(StorageMgr.GUObjectArrayTotalObjectCounter.Get()) / static_cast<float>(MainMenuConf.GUObjectArrayTotalObjectProgressBarTotalValue);
					ImGui::BufferingBar("##ParseGUObjectArray_BufferBar", BarValue, ImVec2(400, 30), bg, col, BarStr.c_str());
				}
			}
		}
	}
}