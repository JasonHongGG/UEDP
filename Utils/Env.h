#pragma once
#include <string>

namespace Const
{
	// Logo
	inline const std::string LogoPathStr = "./logo/main.png";

	// Process Info
	/*inline const std::wstring ProcessName = L"EnderMagnoliaSteamEA-Win64-Shipping.exe";
	inline const std::wstring WindowName = L"EnderMagnolia  ";*/

	inline const std::wstring ProcessName = L"Content Warning.exe";
	inline const std::wstring WindowName = L"Content Warning";

	inline bool PrintAssemblyCode = false;
	inline bool PrintDumpInfo = false;
	inline std::string DisassemblerMode = "BeaEngine"; // Capstone | BeaEngine 

	// Parsing Option
	inline size_t MaxObjectArray = 30;
	inline size_t MaxObjectQuantity = 50000;
}