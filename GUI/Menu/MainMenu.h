#pragma once
#include "../../imgui/imgui.h"

#include "../Config/FontConfig.h"
#include "../Config/MainMenuConfig.h"

#include "../../State/GUIState.h"

#include "../../System/Process.h"

#include "SelectProcessWindow.h"
#include "MainMenu/MainPage.h"
#include "MainMenu/SideBar.h"
#include "MainMenu/ToolPage.h"
#include "MainMenu/ImagePage.h"
#include "MainMenu/FXShaderPage.h"
#include "MainMenu/ConfigPage.h"
#include "MainMenu/ConfigPage/ComponentPanel/ComponentPanelManager.h"
#include "MainMenu/LoadAndSaveSettingPage.h"

namespace MainMenu
{
	inline float Width = 550.f;		
	inline float Height = 650.f;
	inline MainMenuCurPage PageID = MainMenuCurPage::Main;
	inline static bool LastMainMenuFocusIsFalse = false;

	void MainMenuRender();
	void SideBarRender();

	void PositionUpdate();
	void FocusUpdate();
	void StateUpdate();

	void Render()
	{
		SideBarRender();
		MainMenuRender();
		SelectProcessWindow::Render();
		ComponentPanelMgr.Render();
	}
}

void MainMenu::MainMenuRender()
{
	if (PageID == MainMenuCurPage::None) return;
	
	PositionUpdate();
	FocusUpdate();
	ImGui::PushFont(Font::NormalText);
	ImGui::Begin("Jason Hong", NULL, ImGuiWindowFlags_NoTitleBar);	// | ImGuiWindowFlags_NoBackground  ImGuiWindowFlags_NoTitleBar  ImGuiWindowFlags_MenuBar
	{
		MainPage::Render(PageID);
		ToolPage::Render(PageID);
		ImagePage::Render(PageID);
		FXShaderPage::Render(PageID);
		LoadAndSaveSettingPage::Render(PageID);
		ConfigPage::Render(PageID);

		StateUpdate();
	}
	ImGui::End();
	ImGui::PopFont();
}

void MainMenu::SideBarRender()
{
	SideBar::PositionUpdate();
	SideBar::FocusUpdate();
	ImGui::PushFont(Font::NormalText);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, Color::Black);
	ImGui::Begin("MenuSideBar", NULL, ImGuiWindowFlags_NoDecoration | (PageID != MainMenuCurPage::None ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoFocusOnAppearing | DockingMgr.GetDockingDisableFlag());
	{
		SideBar::Render(PageID);
		SideBar::StateUpdate();
	}
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopFont();
}

void MainMenu::PositionUpdate()
{
	ImGui::SetNextWindowSize({ Width,Height }, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos({ (ProcMgr.WindowMgr.UserWindowWidth - Width) / 2, (ProcMgr.WindowMgr.UserWindowHeight - Height) / 2 }, ImGuiCond_FirstUseEver);
	if (MainMenuState.LastPageID == MainMenuCurPage::None) {
		MainMenuState.LastPageID = PageID;
		ImGui::SetNextWindowPos({ MainMenuState.CurrentSideBarPosision.x + 105 , MainMenuState.CurrentSideBarPosision.y }, ImGuiCond_Always);
	}
}

void MainMenu::FocusUpdate()
{
	//來回切一下讓視窗都能夠顯示在最上方
	if (MainMenuState.ShowSideBarWhenFocusMainMenu) {	
		ImGui::SetWindowFocus("Jason Hong");
		MainMenuState.ShowSideBarWhenFocusMainMenu = false;
	}
}

void MainMenu::StateUpdate()
{
	ImGuiIO& io = ImGui::GetIO();

	LastMainMenuFocusIsFalse = (!MainMenuState.FocusOnMainMenu) ? true : false;
	MainMenuState.FocusOnMainMenu = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
	if (MainMenuState.FocusOnMainMenu and LastMainMenuFocusIsFalse) {
		if (!MainMenuState.FocusOnSideBar)
			MainMenuState.ShowSideBarWhenFocusMainMenu = true;
	}

	// Update Window Position
	MainMenuState.CurrentMenuPosision = ImGui::GetWindowPos();

	// Update Window State
	WindowState::IsHookAnyWindow = io.WantCaptureMouse;
	WindowState::IsMousePosValid = ImGui::IsMousePosValid();

	// Update KeyBoard Click Key
	ImGuiKey start_key = (ImGuiKey)0;
	for (ImGuiKey key = start_key; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1)) {
		if ((key >= 0 && key < 512 && ImGui::GetIO().KeyMap[key] != -1) || !ImGui::IsKeyDown(key)) continue;

		MainMenuState.MouseClickKey = key;
	}
}