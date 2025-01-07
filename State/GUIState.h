#pragma once
#include "../imgui/imgui.h"
#include "GlobalState.h"

struct MainMenuStateManager
{
	ImVec2 CurrentMenuPosision = { 0, 0 };
	ImVec2 LastMenuPosision = { 0, 0 };
	bool FocusOnMainMenu = false;
	bool FocusOnMenuSideBar = false;
	bool ShowSideBarWhenFocusMainMenu = false;

	ProcessState OpenProcess = ProcessState::Idle;
	ProcessState CloseProcess = ProcessState::Idle;
	ProcessState SelectProcess = ProcessState::Idle;

	ProcessState ShowBaseAddress = ProcessState::Idle;
	ProcessState Prepareing = ProcessState::Idle;
	ProcessState AutoConfig = ProcessState::Idle;
	ProcessState GetUEVersion = ProcessState::Idle;
	ProcessState GetFNamePool = ProcessState::Idle;
	ProcessState GetGUObjectArray = ProcessState::Idle;
	ProcessState GetGWorld = ProcessState::Idle;
	ProcessState ParseFNamePool = ProcessState::Idle;
	ProcessState ParseGUObjectArray = ProcessState::Idle;
	ProcessState ParseGUObjectArray_ProgressBar = ProcessState::Idle;
};
inline MainMenuStateManager MainMenuState;

struct SelectProcessWindowStateManager
{
	bool OpenSelectProcessWindow = false;
};
inline SelectProcessWindowStateManager SelectProcessWindowState;

struct UEDumperConsoleStateManager
{
	bool OpenUEDumperConsole = false;
};
inline UEDumperConsoleStateManager UEDumperConsole;

struct NotificationStateManager
{
	ProcessState NotifyEvent = ProcessState::Idle;
};
inline NotificationStateManager NotificationState;