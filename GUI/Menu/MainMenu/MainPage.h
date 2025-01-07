#pragma once
#include "../../../imgui/imgui.h"
#include "../../../imgui/imgui_impl_dx11.h"
#include "../../../imgui/imgui_impl_win32.h"

#include "../../../System/Process.h"

#include "../../Config/MainMenuConfig.h"
#include "../../Config/FontConfig.h"
#include "../../Config/TimerConfig.h"

#include "../../../State/GUIState.h"

#include "../../MyGuiComponent.h"

#include "DumperConsole.h"

namespace MainPage
{
	void Render(MainMenuCurPage PageID) {
		if (PageID == MainMenuCurPage::Main) {
			ImGui::BeginChild("Option", ImVec2(0, 0), true);
			{
				ImGui::DummySpace(ImVec2(0, 20));
				ImGui::PushFont(Font::TitleText);
				ImGui::CenterText((!ProcessInfo::ProcessName.empty()) ? ProcessInfo::ProcessName.c_str() : "No Process Selected");
				ImGui::PopFont();

				ImGui::DummySpace(ImVec2(0, 10));
				ImGui::PushFont(Font::TitleText);
				TimerConfig::TimerObj& TempTimeObj = TimerConf.TimeObjMap["DumperTimer"];
				ImGui::Text(
					(TempTimeObj.TimerActivate ? Utils.GetTime(true, TempTimeObj.StartTime, TempTimeObj.EndTime, TempTimeObj.CurTime).c_str()
						: (TempTimeObj.CurTime.time_since_epoch().count() != 0 ? Utils.GetTime(false, TempTimeObj.StartTime, TempTimeObj.EndTime, TempTimeObj.CurTime).c_str()
							: "00:00.000"))
				);	//Timer 啟動 => 顯示當前時間 or 已使用過 Timer => 顯示最後紀錄時間
				ImGui::PopFont();

				// Window Selector
				ImGui::DummySpace();
				{
					ImGui::PushFont(Font::ChineseText);
					ImGui::PushItemWidth(ImGui::GetCurrentWindow()->Size.x - 230);
					MyComboList("##Window Selector", MainMenuConf.WindowTitleSelectIndex, MainMenuConf.WindowTitleList).Show();
					ImGui::PopItemWidth();
					ImGui::PopFont();

					ImGui::SameLine();
					if (ImGui::Button("Select Process")) {
						MainMenuState.SelectProcess = ProcessState::Start;
					}

					ImGui::SameLine();
					if (ImGui::Button("Open Process")) {
						MainMenuState.OpenProcess = ProcessState::Start;
					}
				}

				ImGui::SeparatorText("");
				DumperConsole::Render();
			}
			ImGui::EndChild();
		}
	}
}