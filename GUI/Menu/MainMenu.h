#pragma once
#include "../../imgui/imgui.h"

#include "../Config/FontConfig.h"
#include "../Config/MainMenuConfig.h"

#include "../../State/GUIState.h"

#include "../../System/Process.h"

#include "MainMenu/MainPage.h"
#include "MainMenu/SideBar.h"

namespace MainMenu
{
	inline float Width = 550.f;		
	inline float Height = 650.f;
	inline MainMenuCurPage PageID = MainMenuCurPage::Main;
	inline static bool LastMainMenuFocusIsFalse = false;

	void MainMenuRender();
	void SideBarRender();

	void FocusUpdate();
	void StateUpdate();

	void Render()
	{
		SideBarRender();
		MainMenuRender();
	}
}

void MainMenu::MainMenuRender()
{
	FocusUpdate();
	ImGui::PushFont(Font::NormalText);
	ImGui::SetNextWindowSize({ Width,Height }, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos({ (ProcMgr.WindowMgr.UserWindowWidth - Width) / 2, (ProcMgr.WindowMgr.UserWindowHeight - Height) / 2 }, ImGuiCond_FirstUseEver);
	ImGui::Begin("Jason Hong", NULL, ImGuiWindowFlags_NoTitleBar);	// | ImGuiWindowFlags_NoBackground  ImGuiWindowFlags_NoTitleBar  ImGuiWindowFlags_MenuBar
	{
		MainPage::Render(PageID);

		StateUpdate();
	}
	ImGui::End();
	ImGui::PopFont();
}

void MainMenu::SideBarRender()
{
	SideBar::PositionUpdate();
	SideBar::FocusUpdate();
	ImGui::SetNextWindowSize(ImVec2(90, 0), ImGuiCond_FirstUseEver);
	ImGui::PushFont(Font::NormalText);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, Color::Black);
	ImGui::Begin("MenuSideBar", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | DockingMgr.GetDockingDisableFlag());
	{
		SideBar::Render(PageID);
		SideBar::StateUpdate();
	}
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopFont();
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
	LastMainMenuFocusIsFalse = (!MainMenuState.FocusOnMainMenu) ? true : false;
	MainMenuState.FocusOnMainMenu = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
	if (MainMenuState.FocusOnMainMenu and LastMainMenuFocusIsFalse) {
		if (!MainMenuState.FocusOnMenuSideBar)
			MainMenuState.ShowSideBarWhenFocusMainMenu = true;
	}

	// Update Window Position
	MainMenuState.CurrentMenuPosision = ImGui::GetWindowPos();
}