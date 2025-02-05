#pragma once
#include <vector>
#include <Windows.h>
#include "../../System/Memory.h"
#include "../../System/Process.h"
#include "MonoFunction.h"
#include "MonoUtils.h"
#include "MonoImage.h"
#include "MonoClass.h"

#include "../../imgui/imgui.h"
#include "../../GUI/MyGuiUtils.h"
class MonoManager
{
	MonoNativeFuncSet FunctSet;
	HANDLE hMonoModule = NULL;
	DWORD_PTR GetProcAddressFunctionAddress = 0x0;
	DWORD_PTR RootDomainAddress = 0x0;
	DWORD_PTR AttachAddress = 0x0;
	DWORD_PTR DetachAddress = 0x0;
	std::vector<DWORD_PTR> ThreadFunctionList = { 0, 0, 0 };

	MonoImageAPI* ImageAPI;
	MonoClassAPI* ClassAPI;
public:

	DWORD_PTR FindMonoApiAddress(DWORD_PTR AllocMemoryAddress, std::string FunctionName)
	{
		DWORD_PTR ReturnAddress = AllocMemoryAddress + 0x300;
		DWORD_PTR FunctNameAddress = AllocMemoryAddress + 0x200;

		// Write Function Name
		BYTE* ByteArray = reinterpret_cast<BYTE*>(FunctionName.data());
		MemMgr.MemWriter.WriteBytes(FunctNameAddress, ByteArray, FunctionName.size() + 1);

		// Prepare Code
		std::vector<BYTE> Code = {
			0x48, 0x83, 0xEC, 0x28,											// sub rsp, 28h
			0x48, 0xB9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,		// mov rcx, 0xFF FF FF FF FF FF FF FF
			0x48, 0xBA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,		// mov rdx, 0xFFFFFFFFFFFFFFFF	// ボ蠢传跋办
			0x48, 0xB8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,		// mov rax, 0xFFFFFFFFFFFFFFFF
			0xFF, 0xD0,														// call rax
			0x49, 0xBC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,		// mov r12, 0xFFFFFFFFFFFFFFFF
			0x49, 0x89, 0x04, 0x24,											// mov qword ptr [r12], rax
			0x48, 0x83, 0xC4, 0x28,											// add rsp, 28h
			0xC3															// ret
		};
		MonoUtils.PatchAddress(Code, { 6, 16, 26, 38 }, {
				(DWORD_PTR)hMonoModule,			// 蠢传 rcx
				FunctNameAddress,				// 蠢传 rdx
				GetProcAddressFunctionAddress,  // 蠢传 rax
				ReturnAddress					// 蠢传 r12
		});

		// Execute Code
		MemMgr.MemWriter.WriteBytes(AllocMemoryAddress, Code.data(), Code.size());
		MemMgr.RegionEnumerator.CreateRemoteThreadAndExcute(ProcessInfo::hProcess, AllocMemoryAddress);

		DWORD_PTR FunctionAddress = 0x0;
		MemMgr.MemReader.ReadMem(FunctionAddress, ReturnAddress);
		return FunctionAddress;
	}


	void BuildMonoFunctSet()
	{
		FunctSet = MonoNativeFuncSet();
		DWORD_PTR AllocMemoryAddress = MemMgr.RegionEnumerator.MemoryAlloc(ProcessInfo::hProcess);
		for (const std::string functName : mono_native_func_name) {
			FunctSet.FunctPtrSet[functName]->FunctionAddress = FindMonoApiAddress(AllocMemoryAddress, functName);
		}

		MemMgr.RegionEnumerator.MemoryFree(ProcessInfo::hProcess, AllocMemoryAddress);
	}

	void GetRootDomain()
	{
		RootDomainAddress = (DWORD_PTR)FunctSet.FunctPtrSet["mono_get_root_domain"]->Call<DWORD_PTR>(CALL_TYPE_CDECL);
	}

	bool Initialized = false;
	void Init()
	{
		HMODULE hModule = ProcMgr.ModuleMgr.GetModule(ProcessInfo::PID, L"KERNEL32.dll");
		GetProcAddressFunctionAddress = ProcMgr.ModuleMgr.GetFunctionAddress(ProcessInfo::hProcess, hModule, "GetProcAddress");

		hMonoModule = ProcMgr.ModuleMgr.GetModule(ProcessInfo::PID, L"mono.dll");
		if (!hMonoModule) hMonoModule = ProcMgr.ModuleMgr.GetModule(ProcessInfo::PID, L"mono-2.0-bdwgc.dll");
		BuildMonoFunctSet();
		GetRootDomain();

		AttachAddress = FunctSet.FunctPtrSet["mono_thread_attach"]->FunctionAddress;
		DetachAddress = FunctSet.FunctPtrSet["mono_thread_detach"]->FunctionAddress;
		ThreadFunctionList = { RootDomainAddress, AttachAddress, DetachAddress };

		ImageAPI = new MonoImageAPI(&FunctSet, &ThreadFunctionList);
		ClassAPI = new MonoClassAPI(ImageAPI, &FunctSet, &ThreadFunctionList);
		
	}

	DWORD_PTR CurrentPlagerInstance = 0x0;
	DWORD_PTR BotListInstance = 0x0;
	MonoClass* UnityEngineComponentClass;
	MonoClass* UnityEngineTransformClass;
	MonoClass* UnityEngineCameraClass;
	MonoMethod* GetTransformMethod;
	MonoMethod* GetPositionMethod;
	MonoMethod* WorldToScreenPointMethod;
	void Test()
	{
		
		// Bot
		MonoClass* BotHandlerClass = ClassAPI->FindClassInImageByName("Assembly-CSharp", "BotHandler");
		BotHandlerClass->Instance = BotHandlerClass->FindField("instance")->GetValue<DWORD_PTR>();
		BotListInstance = BotHandlerClass->FindField("bots")->GetValue<DWORD_PTR>();
		printf("BotListInstance 0x%llx\n", BotListInstance);

		// Player
		MonoClass* PlayerHandlerClass = ClassAPI->FindClassInImageByName("Assembly-CSharp", "PlayerHandler");
		PlayerHandlerClass->Instance = PlayerHandlerClass->FindField("instance")->GetValue<DWORD_PTR>();
		DWORD_PTR PlayerListInstance = PlayerHandlerClass->FindField("players")->GetValue<DWORD_PTR>();
		DWORD_PTR PlayerListBaseAddress = 0x0;
		MemMgr.MemReader.ReadMem<DWORD_PTR>(PlayerListBaseAddress, PlayerListInstance + 0x10);
		int PlayerListSize = 0;
		MemMgr.MemReader.ReadMem<int>(PlayerListSize, PlayerListInstance + 0x18);
		std::vector<DWORD_PTR> PlayerList = MemMgr.MemReader.ReadArray<DWORD_PTR>(PlayerListBaseAddress + 0x20, PlayerListSize);
		CurrentPlagerInstance = PlayerList[0];

		// Transform Method
		UnityEngineComponentClass = ClassAPI->FindClassInImageByName("UnityEngine.CoreModule", "UnityEngine.Component");
		GetTransformMethod = UnityEngineComponentClass->FindMethod("get_transform");
		UnityEngineTransformClass = ClassAPI->FindClassInImageByName("UnityEngine.CoreModule", "UnityEngine.Transform");
		GetPositionMethod = UnityEngineTransformClass->FindMethod("get_position_Injected");
		UnityEngineCameraClass = ClassAPI->FindClassInImageByName("UnityEngine.CoreModule", "UnityEngine.Camera");
		UnityEngineCameraClass->Instance = UnityEngineCameraClass->FindMethod("get_main")->Call< DWORD_PTR >();
		WorldToScreenPointMethod = UnityEngineCameraClass->FindMethod("WorldToScreenPoint_Injected");

		

		printf("BotListInstance 0x%llx\n", BotListInstance);
		Initialized = true;
	}

	void TestLoop(HWND window)
	{
		if (!Initialized) return;
		// Player
		GetTransformMethod->Class->Instance = CurrentPlagerInstance;
		GetPositionMethod->Class->Instance = GetTransformMethod->Call<DWORD_PTR>();
		DWORD_PTR AllocMemoryAddress = MemMgr.RegionEnumerator.MemoryAlloc(ProcessInfo::hProcess);
		GetPositionMethod->Call<float>(AllocMemoryAddress);
		std::vector<float> PlayerWorldPos = MemMgr.MemReader.ReadArray<float>(AllocMemoryAddress, 3);
		MemMgr.RegionEnumerator.MemoryFree(ProcessInfo::hProcess, AllocMemoryAddress);
		if (PlayerWorldPos[0] == 0) return;



		// Bot List
		DWORD_PTR BotListBaseAddress = 0x0;
		MemMgr.MemReader.ReadMem<DWORD_PTR>(BotListBaseAddress, BotListInstance + 0x10);
		int BotListSize = 0;
		MemMgr.MemReader.ReadMem<int>(BotListSize, BotListInstance + 0x18);
		std::vector<DWORD_PTR> BotList = MemMgr.MemReader.ReadArray<DWORD_PTR>(BotListBaseAddress + 0x20, BotListSize);
		if (BotList.size() <= 1) return;

		std::vector<std::vector<float>> BotWorldPos;
		std::vector<std::vector<float>> BotScreenPos;
		for (int i = 0; i < BotList.size(); i++) {
			// World Pos
			GetTransformMethod->Class->Instance = BotList[i];
			GetPositionMethod->Class->Instance = GetTransformMethod->Call<DWORD_PTR>();
			DWORD_PTR AllocMemoryAddress = MemMgr.RegionEnumerator.MemoryAlloc(ProcessInfo::hProcess);
			GetPositionMethod->Call<float>(AllocMemoryAddress);
			std::vector<float> WorldPos = MemMgr.MemReader.ReadArray<float>(AllocMemoryAddress, 3);
			MemMgr.RegionEnumerator.MemoryFree(ProcessInfo::hProcess, AllocMemoryAddress);
			BotWorldPos.push_back(WorldPos);

			//Screen Pos
			AllocMemoryAddress = MemMgr.RegionEnumerator.MemoryAlloc(ProcessInfo::hProcess);
			MemMgr.MemWriter.WriteArray<float>(AllocMemoryAddress, WorldPos);
			WorldToScreenPointMethod->Call<float>(AllocMemoryAddress, 2, AllocMemoryAddress + 0x100);
			std::vector<float> ScreenPos = MemMgr.MemReader.ReadArray<float>(AllocMemoryAddress + 0x100, 3);
			MemMgr.RegionEnumerator.MemoryFree(ProcessInfo::hProcess, AllocMemoryAddress);
			BotScreenPos.push_back(ScreenPos);

			// Draw
			RECT windowRect;
			GetWindowRect(window, &windowRect);
			float windowWidth = float(windowRect.right - windowRect.left);
			float windowHeight = float(windowRect.bottom - windowRect.top);
			ImDrawList* drawList = ImGui::GetBackgroundDrawList();
			drawList->AddCircle(ImVec2(windowRect.left + ScreenPos[0], windowRect.bottom - ScreenPos[1]), 10, IM_COL32(255, 0, 0, 255));

			// Distance
			float distance = sqrt(pow(PlayerWorldPos[0] - WorldPos[0], 2) + pow(PlayerWorldPos[1] - WorldPos[1], 2) + pow(PlayerWorldPos[2] - WorldPos[2], 2));
			std::string distanceStr = std::to_string(distance);
			drawList->AddText(ImVec2(windowRect.left + ScreenPos[0] - GUIUtils.GetStringWidth(distanceStr) / 2, windowRect.top + ScreenPos[1] + 10), IM_COL32(255, 255, 255, 255), distanceStr.c_str());
		}
		Sleep(10);
	}
};

inline MonoManager MonoMgr = MonoManager();