#include "Menu.h"
#include "Include/Notify/imgui_notify.h"
#include "TextureLoader.h"
#include "Config/NotificationConfig.h"
#include "Config/LogoConfig.h"
#include "../State/GUIState.h"
#include "Menu/MainMenu.h"
#include "Menu/MainConsole.h"


void Menu::MenuEntry() 
{
	if (InitialFlag) Initial();
	MainMenu::Render();
	MainConsole::Render();

	// ====================================================
	//Notification
	if (NotificationState.NotifyEvent == ProcessState::Start) {
		NotificationState.NotifyEvent = ProcessState::Processing;
		ImGuiToastType_ Type = ImGuiToastType_None;
		if (NotificationConf.Type == NotificationConfig::NotiyType::None)			Type = ImGuiToastType_None;
		else if (NotificationConf.Type == NotificationConfig::NotiyType::Success)	Type = ImGuiToastType_Success;
		else if (NotificationConf.Type == NotificationConfig::NotiyType::Warning)	Type = ImGuiToastType_Warning;
		else if (NotificationConf.Type == NotificationConfig::NotiyType::Error)		Type = ImGuiToastType_Error;
		else if (NotificationConf.Type == NotificationConfig::NotiyType::Info)		Type = ImGuiToastType_Info;
		ImGuiToast toast(Type, NotificationConf.DismissTime);
		toast.set_title(NotificationConf.Title.c_str());
		toast.set_content(NotificationConf.Content.c_str());
		ImGui::InsertNotification(toast);
		NotificationState.NotifyEvent = ProcessState::Completed;
	}

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 255));
	ImGui::RenderNotifications(Font::NotifyText, Font::NotifyContentText);
	ImGui::PopStyleColor();
}

void Menu::Initial() {
	InitialFlag = false;
	GetLogo();
	SetStyle();
}

void Menu::GetLogo() {
	bool ret = ImageLoader.LoadTextureFromFile(D3DRenderMgr.Device, (LogConf.LogoPathStr).c_str(), LogConf.LogoTextureVec, &LogConf.LogoWidth, &LogConf.LogoHeight, &LogConf.LogoFrame);
	LogConf.LogoProportion = (float)LogConf.LogoWidth / (float)LogConf.LogoHeight;
}

void Menu::SetStyle() {
	// Setting styles
	ImGuiStyle* StyleMgr = &ImGui::GetStyle();

	// Sizes
	StyleMgr->FramePadding = ImVec2(5, 5);
	StyleMgr->FrameBorderSize = 1.f;
	StyleMgr->WindowPadding = ImVec2(6, 6);
	StyleMgr->GrabRounding = 0.f;
	StyleMgr->GrabMinSize = 20.f;
	StyleMgr->ButtonTextAlign = ImVec2(0.5, 0.5);
	StyleMgr->ItemSpacing = ImVec2(9, 4);

	// Colour setup
	ImColor darkGrey = ImColor(29, 31, 31, 255);
	ImColor lightGrey = ImColor(38, 42, 43, 255);
	ImColor lightGreyTrans = ImColor(38, 42, 43, 175);
	ImColor darkBlue = ImColor(52, 98, 237, 255);
	ImColor lightBlue = ImColor(78, 139, 246, 255);

	// Colours
	StyleMgr->Colors[ImGuiCol_FrameBg] = lightGrey;				//
	StyleMgr->Colors[ImGuiCol_FrameBgHovered] = darkGrey;
	StyleMgr->Colors[ImGuiCol_FrameBgActive] = darkGrey;
	StyleMgr->Colors[ImGuiCol_TitleBgActive] = darkBlue;			//
	StyleMgr->Colors[ImGuiCol_TitleBgCollapsed] = lightGreyTrans;
	StyleMgr->Colors[ImGuiCol_ChildBg] = darkGrey;					//
	StyleMgr->Colors[ImGuiCol_MenuBarBg] = lightGrey;				//
	StyleMgr->Colors[ImGuiCol_WindowBg] = lightGrey;				//
	StyleMgr->Colors[ImGuiCol_CheckMark] = darkBlue;				//
	StyleMgr->Colors[ImGuiCol_SliderGrab] = darkBlue;				//
	StyleMgr->Colors[ImGuiCol_SliderGrabActive] = lightBlue;
	StyleMgr->Colors[ImGuiCol_ResizeGrip] = darkBlue;				//
	StyleMgr->Colors[ImGuiCol_ResizeGripHovered] = lightBlue;
	StyleMgr->Colors[ImGuiCol_HeaderHovered] = darkBlue;			//
	StyleMgr->Colors[ImGuiCol_HeaderActive] = lightBlue;

	//StyleMgr->Colors[ImGuiCol_Button] = Color::LightBlue;
	//StyleMgr->Colors[ImGuiCol_ButtonHovered] = Color::LightBlue;
	StyleMgr->Colors[ImGuiCol_Button] = darkBlue;
	StyleMgr->Colors[ImGuiCol_ButtonHovered] = lightBlue;

	StyleMgr->TabRounding = 8.f;
	StyleMgr->FrameRounding = 8.f;
	StyleMgr->GrabRounding = 8.f;
	StyleMgr->WindowRounding = 8.f;
	StyleMgr->PopupRounding = 8.f;
	StyleMgr->ChildRounding = 8.f;
}