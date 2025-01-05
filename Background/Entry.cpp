#pragma once
#include "Entry.h"
#include <iomanip>
#include <chrono>
#include <ctime>
#include "d3d11.h"
#include "../imgui/imgui.h"

//#include "BaseAddress.h"
//#include "DumperUtils.h"
//#include "UEOffset.h"				//AutoConfig
//#include "Console.h"
//#include "../json/MyJson.h"
#include "../Utils/Utils.h"
#include "../Utils/Env.h"




void BackgroundEntry::InitialProcess()
{
	// initial
	StorageMgr.SetUEVersion(0);
	StorageMgr.SetGWorld(0);
	StorageMgr.SetGUObjectArray(0);
	StorageMgr.SetFNamePool(0);

	// ==========  Initial String Parameter  ==========
	std::wstring WindowName;
	if (Menu_MainConf.WindowTitle_SelectIndex != 0) {
		ProcMgr.infoMgr.GetProcessNameByPID(ProcessWindow::WindowList[Menu_MainConf.WindowTitle_SelectIndex].ProcessID);
		std::wstring WideStr(ProcessWindow::WindowList[Menu_MainConf.WindowTitle_SelectIndex].WindowTitle.begin(), ProcessWindow::WindowList[Menu_MainConf.WindowTitle_SelectIndex].WindowTitle.end());
		ProcessWindow::WindowName = Utils.UnicodeToUTF8(WideStr.c_str());
	}
	else {
		ProcessInfo::ProcessName = Utils.UnicodeToUTF8(Const::ProcessName.c_str());
		ProcessWindow::WindowName = Utils.UnicodeToUTF8(Const::WindowName.c_str());
	}

	std::wstring L_ProcessName = Utils.UTF8ToUnicode(ProcessWindow::WindowName.c_str());

	// ==========  Get Process ID、Handler  ==========
	if (!ProcMgr.infoMgr.GetPID(Utils.UTF8ToUnicode(ProcessInfo::ProcessName.c_str()).c_str())) {
		MsgHandler.NotifyEvent(NotificationManager::NotiyType::Error, "Process Not Running !!!", "..."); 
		return; 
	}

	if (ProcessInfo::hProcess != 0) CloseHandle(ProcessInfo::hProcess);		// 如果要換 Process，要把前一個 Process Handler 關閉
	ProcessInfo::hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)ProcessInfo::PID);
	ProcMgr.infoMgr.ProcessIs64Bit();
	printf("[ Process ID ] %p\n", (void*)ProcessInfo::PID);
	printf("[ Process Handler ] %p\n", (void*)ProcessInfo::hProcess);
	printf("\n");


	// ==========  Get Module、Module Base  ==========
	ProcMgr.moduleMgr.GetModule(ProcessInfo::PID, L_ProcessName.c_str());
	DWORD_PTR base = ProcMgr.moduleMgr.GetModuleBaseAddress(ProcessInfo::PID, L_ProcessName.c_str());
	DWORD size = ProcMgr.moduleMgr.GetModuleSize(ProcessInfo::hProcess, ProcessModule::hMainMoudle);
	printf("[ Module Obj Address ] %p\n", ProcessModule::hMainMoudle);		// module handler 取得的就是 module base address
	printf("[ Module Base Address ] %p\n", (void*)base);
	printf("[ Module Size ] %X\n", size);
	printf("\n");


	// ==========  Get Window Handler  ==========
	std::vector<HWND> windowsHandler;
	ProcMgr.windowMgr.GetWindowsFromProcessID((DWORD)ProcessInfo::PID, windowsHandler, ProcessWindow::WindowName.c_str());
	ProcMgr.windowMgr.GetWindowsNameFromHWND(windowsHandler[0]);
	wprintf(L"[ windowTitle ] %ls\n", L_ProcessName.c_str());
	printf("[ WindowsHandler Size ] %d\n", (int)windowsHandler.size());
	printf("\n");


	// ==========  Test Area  ==========


}

void BackgroundEntry::CloseProcess()
{
	CloseHandle(ProcessInfo::hProcess);
	OverlayMgr.MenuState = false;
}
