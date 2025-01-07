#pragma once
#include "../../Config/MainMenuConfig.h"
#include "../../Config/FontConfig.h"
#include "../../Config/ColorConfig.h"
#include "../../Config/LogoConfig.h"
#include "../../Config/StyleConfig.h"

#include "../../../State/GUIState.h"

#include "../../DockingManager.h"

namespace SideBar
{
	inline MainMenuCurPage* PageID = nullptr;

	void ButtonShadow(bool IsFilled, ImVec4 Color, ImVec2 CursorPos, ImVec2 ButtonSize, float FramePadding = 0);

	void MenuSideBarComponent(std::string Name, MainMenuCurPage Type, ImVec2 ButtonSize, int& idx);

	void StateUpdate();

	void FocusUpdate();

	void PositionUpdate();

	void Render(MainMenuCurPage& PageID)
	{
		SideBar::PageID = &PageID;

		// Logo
		static float ImageWidth = ImGui::GetContentRegionAvail().x;
		static float ImageHeight = static_cast<int>(std::round(ImageWidth / LogConf.LogoProportion));
		if (LogConf.LogoTextureVec.size() > 0)
			ImGui::Image((void*)LogConf.LogoTextureVec[0], ImVec2(ImageWidth, ImageHeight));	// no consider gif

		// Option
		int idx = 0;
		ImVec2 ButtonSize = ImVec2(65, 60);
		Style::DefaultBtnStyleSwitch(true);
		ImGui::PushFont(Font::BigIconText);
		MenuSideBarComponent(ICON_FA_TASKS"##Main", MainMenuCurPage::Main, ButtonSize, idx);
		MenuSideBarComponent(ICON_FA_TV_ALT"##DumperConsole", MainMenuCurPage::DumperConsole, ButtonSize, idx);
		MenuSideBarComponent(ICON_FA_TOOLS"##Tool", MainMenuCurPage::Tool, ButtonSize, idx);
		MenuSideBarComponent(ICON_FA_IMAGE"##Image", MainMenuCurPage::Image, ButtonSize, idx);
		MenuSideBarComponent(ICON_FA_MAGIC"##Fx", MainMenuCurPage::FX, ButtonSize, idx);
		MenuSideBarComponent(ICON_FA_SAVE"##SL", MainMenuCurPage::MenuConfigSL, ButtonSize, idx);
		MenuSideBarComponent(ICON_FA_GEAR"##Config", MainMenuCurPage::MainConfig, ButtonSize, idx);
		MenuSideBarComponent(ICON_FA_POWER_OFF"##Quit", MainMenuCurPage::Quit, ButtonSize, idx);
		ImGui::DummySpace();
		ImGui::PopFont();
		Style::DefaultBtnStyleSwitch(false);
	}
}

void SideBar::ButtonShadow(bool IsFilled, ImVec4 Color, ImVec2 CursorPos, ImVec2 ButtonSize, float FramePadding)
{
	ImVec2 CursorPosBak = ImGui::GetCursorPos();
	CursorPos.x = (ImGui::GetWindowWidth() - ButtonSize.x) / 2;
	CursorPos.y += FramePadding + 3;
	ImGui::SetCursorPos(CursorPos);
	ImGui::ShadowRectComponent(IsFilled, Color, 100.f, ImVec2(0, 0), 0, ButtonSize);
	ImGui::SetCursorPos(CursorPosBak);
}

void SideBar::MenuSideBarComponent(std::string Name, MainMenuCurPage Type, ImVec2 ButtonSize, int& idx)
{
	static ImGuiStyle* StyleMgr = &ImGui::GetStyle();
	static bool ColorState = false;
	ColorState = (*PageID == Type or (Type == MainMenuCurPage::DumperConsole and UEDumperConsole.OpenUEDumperConsole));
	ImVec2 CursorPos = ImGui::GetCursorPos();

	ImGui::DummySpace();
	if (ColorState) Style::ActivateBtnStyleSwitch(true);
	if (ImGui::CenterButton(Name.c_str(), ButtonSize)) {
		if (Type == MainMenuCurPage::DumperConsole)
			UEDumperConsole.OpenUEDumperConsole = !UEDumperConsole.OpenUEDumperConsole;
		else if (Type == MainMenuCurPage::Quit)
			MainMenuState.CloseProcess = ProcessState::Start;
		else
			*PageID = Type;
	}
	if (ImGui::IsItemHovered())
		ButtonShadow(true, Color::LightBlue, CursorPos, ButtonSize, StyleMgr->FramePadding.x);

	if (ColorState) {
		Style::ActivateBtnStyleSwitch(false);
		ButtonShadow(false, Color::LightBlue, CursorPos, ButtonSize, StyleMgr->FramePadding.x);
	}
	idx++;
}


void SideBar::StateUpdate()
{
	MainMenuState.FocusOnMenuSideBar = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
}

void SideBar::FocusUpdate()
{
	if (MainMenuState.ShowSideBarWhenFocusMainMenu) {
		ImGui::SetWindowFocus("MenuSideBar");
	}
}

void SideBar::PositionUpdate()
{
	if (MainMenuState.CurrentMenuPosision != MainMenuState.LastMenuPosision) {
		ImGui::SetNextWindowPos({ (MainMenuState.CurrentMenuPosision.x - 105/*padding*/), MainMenuState.CurrentMenuPosision.y });
		MainMenuState.LastMenuPosision = MainMenuState.CurrentMenuPosision;
	}
}