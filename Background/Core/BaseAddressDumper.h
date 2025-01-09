#pragma once

#pragma once
#define WIN32_LEAN_AND_MEAN 

#include <vector>
#include <iostream>
#include <Windows.h>
#include "../../System/Process.h"
#include "../../System/Memory.h"
#include "../../Utils/Env.h"
#include "Utils.h"
#include "../Storage/StorageManager.h"
#include "../../State/GUIState.h"

class BaseAddressDumperClass
{
public:
	BaseAddressDumperClass() {};
	~BaseAddressDumperClass() {};

	std::vector<DWORD_PTR> FindAddress(DWORD_PTR Address);

	bool ValidateFNamePool(DWORD_PTR Address, DWORD_PTR& FNamePoolBaseAddress, size_t& FNamePool_FirstEleOffest);

	void GetFNamePool();

	bool ValidateGUObjectArray(DWORD_PTR Address, DWORD_PTR& GUObjectArrayBaseAddress, size_t& GUObjectArray_ArrEleSize);

	void GetGUObjectArray();

	void GetGWorld();

	ProcessState ShowBaseAddress();

private:
};

inline BaseAddressDumperClass BaseAddressDumper = BaseAddressDumperClass();