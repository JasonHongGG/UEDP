#pragma once
#include "Entry.h"
#include <iomanip>
#include <chrono>
#include <ctime>
#include "d3d11.h"
#include "../imgui/imgui.h"

#include "../GUI/Config/SelectProcessWindowConfig.h"
#include "../State/GUIState.h"
#include "../State/EventHandler.h"

#include "Entry/ParseEntry.h"
#include "Entry/ConsoleEntry.h"
#include "Entry/PackageViewEntry.h"
#include "Entry/InspectorEntry.h"
#include "Entry/APIEntry.h"
#include "Entry/LoadAndSaveSettingEntry.h"
//#include "Console.h"
//#include "../json/MyJson.h"
#include "../Utils/Utils.h"
#include "../Utils/Env.h"




void BackgroundEntry::InitialProcess()
{
	// initial
	//StorageMgr.SetUEVersion(0);
	//StorageMgr.SetGWorld(0);
	//StorageMgr.SetGUObjectArray(0);
	//StorageMgr.SetFNamePool(0);

	// ==========  Initial String Parameter  ==========
	std::wstring WindowName;
	if ( SelectProcessWindowConf.WindowTitleSelectIndex != 0 ) {
		ProcMgr.InfoMgr.GetProcessNameByPID(ProcessWindow::WindowList[SelectProcessWindowConf.WindowTitleSelectIndex].ProcessID);
		std::wstring WideStr(ProcessWindow::WindowList[SelectProcessWindowConf.WindowTitleSelectIndex].WindowTitle.begin(), ProcessWindow::WindowList[SelectProcessWindowConf.WindowTitleSelectIndex].WindowTitle.end());
		ProcessWindow::WindowName = Utils.UnicodeToUTF8(WideStr.c_str());
	}
	else {
		ProcessInfo::ProcessName = Utils.UnicodeToUTF8(Const::ProcessName.c_str());
		ProcessWindow::WindowName = Utils.UnicodeToUTF8(Const::WindowName.c_str());
	}

	std::wstring L_ProcessName = Utils.UTF8ToUnicode(ProcessWindow::WindowName.c_str());

	// ==========  Get Process ID、Handler  ==========
	if (!ProcMgr.InfoMgr.GetPID(Utils.UTF8ToUnicode(ProcessInfo::ProcessName.c_str()).c_str())) {
		EventHandler::NotifyEvent(NotificationConfig::NotiyType::Error, "Process Not Running !!!", "..."); 
		return; 
	}

	if (ProcessInfo::hProcess != 0) CloseHandle(ProcessInfo::hProcess);		// 如果要換 Process，要把前一個 Process Handler 關閉
	ProcessInfo::hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)ProcessInfo::PID);
	ProcMgr.InfoMgr.ProcessIs64Bit();
	printf("[ Process ID ] %p\n", (void*)ProcessInfo::PID);
	printf("[ Process Handler ] %p\n", (void*)ProcessInfo::hProcess);
	printf("\n");


	// ==========  Get Module、Module Base  ==========
	ProcMgr.ModuleMgr.GetModule(ProcessInfo::PID, L_ProcessName.c_str());
	DWORD_PTR base = ProcMgr.ModuleMgr.GetModuleBaseAddress(ProcessInfo::PID, L_ProcessName.c_str());
	DWORD size = ProcMgr.ModuleMgr.GetModuleSize(ProcessInfo::hProcess, ProcessModule::hMainMoudle);
	printf("[ Module Obj Address ] %p\n", ProcessModule::hMainMoudle);		// module handler 取得的就是 module base address
	printf("[ Module Base Address ] %p\n", (void*)base);
	printf("[ Module Size ] %X\n", size);
	printf("\n");


	// ==========  Get Window Handler  ==========
	std::vector<HWND> windowsHandler;
	ProcMgr.WindowMgr.GetWindowsFromProcessID((DWORD)ProcessInfo::PID, windowsHandler, ProcessWindow::WindowName.c_str());
	ProcMgr.WindowMgr.GetWindowsNameFromHWND(windowsHandler[0]);
	wprintf(L"[ windowTitle ] %ls\n", L_ProcessName.c_str());
	printf("[ WindowsHandler Size ] %d\n", (int)windowsHandler.size());
	printf("\n");


	// ==========  Test Area  ==========


}

void BackgroundEntry::CloseProcess()
{
	CloseHandle(ProcessInfo::hProcess);
	Process::ProcState = Process::ProcessState::Stop;
}


void BackgroundEntry::Entry()
{
	// ==========  Initial Process  ==========
	if (MainMenuState.OpenProcess == ProcessState::Start) {
		MainMenuState.OpenProcess = ProcessState::Processing;
		EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] OpenProcess");
		InitialProcess();
		MainMenuState.OpenProcess = ProcessState::Completed;
	}

	if (MainMenuState.CloseProcess == ProcessState::Start) {
		MainMenuState.CloseProcess = ProcessState::Processing;
		EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] CloseProcess");
		CloseProcess();
		MainMenuState.CloseProcess = ProcessState::Completed;
	}

	if (MainMenuState.SelectProcess == ProcessState::Start) {
		MainMenuState.SelectProcess = ProcessState::Processing;
		EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] SelectProcess");
		
		ProcMgr.WindowMgr.GetWindowList();
		SelectProcessWindowConf.WindowTitleList.clear();
		for (int i = 0; i < ProcessWindow::WindowList.size(); i++) {
			SelectProcessWindowConf.WindowTitleList.push_back(ProcessWindow::WindowList[i].WindowTitle);
		}

		SelectProcessWindowState.OpenSelectProcessWindow = true;
		MainMenuState.SelectProcess = ProcessState::Completed;
	}

	// Already Attach Process
	if (ProcessWindow::MainWindow) 
	{
		ParseEntry::Entry();
		ConsoleEntry::Entry();
		PackageViewEntry::Entry();
		InspectorEntry::Entry();
		APIEntry::Entry();
	}
	APIEntry::FileEntry();
	LoadAndSaveSettingEntry::Entry();
}