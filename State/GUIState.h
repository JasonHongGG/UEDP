#pragma once
#include <string>
#include <vector>
#include "../imgui/imgui.h"
#include "GlobalState.h"
#include "EventHandler/MessageInterface.h"

struct MainMenuStateManager
{
	ImVec2 CurrentMenuPosision = { 0, 0 };
	ImVec2 LastMenuPosision = { 0, 0 };
	bool FocusOnMainMenu = false;
	bool FocusOnSideBar = false;
	bool ShowSideBarWhenFocusMainMenu = false;
	bool ShowMainMenuWhenFocusSideBar = false;

	int MouseClickKey = 0;

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




struct MainConsoleStateManager
{
	bool OpenDumperConsole = false;
	bool OpenSettingPanel = false;

	ProcessState FNameInfoGetEvent = ProcessState::Idle;
	ProcessState GUObjectInfoGetEvent = ProcessState::Idle;
	ProcessState GUObjectInfoAddEvent = ProcessState::Idle;
	ProcessState GUObjectInfoParseEvent = ProcessState::Idle;
	ProcessState GUObjectInfoExpandEvent = ProcessState::Idle;
	MessageObject GUObjectInfoExpandMsg = MessageObject();
};
inline MainConsoleStateManager MainConsoleState;

struct PackageViwerStateManager
{
	MessageObject ObjectDataUpdateEvent = MessageObject();
	MessageObject ObjectContentCreatEvent = MessageObject();
	MessageObject SearchUpdateEvent = MessageObject();
	MessageObject SearchShowEvent = MessageObject();
	
};
inline PackageViwerStateManager PackageViwerState;

struct InspectorStateManager
{
	MessageObject GetFNameEvent = MessageObject();
	MessageObject ObjectContentCreatEvent = MessageObject();
	MessageObject ObjectContentUpdateEvent = MessageObject();
	MessageObject ObjectContentDeleteEvent = MessageObject();
};
inline InspectorStateManager InspectorState;

struct APIStateManager
{
	struct SelfMessageObject : public MessageObject {
		std::vector<std::pair<DWORD_PTR, size_t>> DepthPath;
		std::string FilePath = "";
	};
	SelfMessageObject AddEvent = SelfMessageObject();
	SelfMessageObject OpenFileEvent = SelfMessageObject();
	SelfMessageObject SaveFileEvent = SelfMessageObject();
	MessageObject UpdateEvent = MessageObject();
};
inline APIStateManager APIState;




struct NotificationStateManager
{
	ProcessState NotifyEvent = ProcessState::Idle;
};
inline NotificationStateManager NotificationState;




struct ConfigPageComponentStateManager
{
	bool OpenShadowEditor = false;
	bool OpenToggleEditor = false;
	bool OpenGradientEditor = false;
};
inline ConfigPageComponentStateManager ConfigPageComponentStateMgr;




struct LoadAndSaveSettingStateManager
{
	std::string CurTime = "";
	ProcessState SaveEvent = ProcessState::Idle;
	ProcessState ReadEvent = ProcessState::Idle; 
	ProcessState DeleteEvent = ProcessState::Idle;
	ProcessState LoadEvent = ProcessState::Idle;
};
inline LoadAndSaveSettingStateManager LoadAndSaveSettingStateMgr;




struct ProgressBarStateManager
{
	ProcessState FNamePoolEvent = ProcessState::Idle;
	ProcessState GUObjectArrayEvent = ProcessState::Idle;
};
inline ProgressBarStateManager ProgressBarStateMgr;