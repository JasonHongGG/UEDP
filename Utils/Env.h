#pragma once
#include <string>

namespace Const
{
	// Logo
	inline const std::string LogoPathStr = "./logo/main.png--";

	// Process Info
	inline const std::wstring ProcessName = L"EnderMagnoliaSteamEA-Win64-Shipping.exe";
	inline const std::wstring WindowName = L"EnderMagnolia  ";

	//inline const std::wstring ProcessName = L"Palworld-Win64-Shipping.exe";
	//inline const std::wstring WindowName = L"Pal  ";

	//const wchar_t* ProgramName = L"Tutorial-i386.exe";
	//const wchar_t* WindowName = L"Step 2";

	inline bool PrintAssemblyCode = false;
	inline bool PrintDumpInfo = false;

	// Parsing Option
	inline size_t MaxObjectArray = 30;
	inline size_t MaxObjectQuantity = 50000;
}