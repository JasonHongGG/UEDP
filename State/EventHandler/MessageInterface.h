#pragma once
#include <Windows.h>
#include <string>
#include "../GlobalState.h"

class MessageObject {
public:
	ProcessState State = ProcessState::Idle;
	std::string Str = "";
	std::string SearchStr = "";
	std::string Name = "";
	std::string Type = "";
	std::string PackageName = "";
	std::string TabName = "";
	std::string ObjectName = "";
	std::string CurTime = "";
	DWORD_PTR Address = 0x0;
	DWORD_PTR BaseAddress = 0x0;
	DWORD_PTR ObjectAddress = 0x0;
	DWORD_PTR InstanceAddress = 0x0;
	size_t Size = 1;
	size_t Offset = 0x0;
	int Bit = -1;
	int Index = 0;
	int FNameID = 0;
	bool IsInstance = false;
	bool EditorEnable = false;
private:
};