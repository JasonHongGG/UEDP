#pragma once
#include <vector>
#include <Windows.h>
#include "../../System/Memory.h"
#include "../../System/Process.h"
#include "UnityFunction.h"
#include "UnityUtils.h"
#include "UnityImage.h"
#include "MonoClass.h"

class CString
{
public:
	DWORD_PTR Address = 0x0;
	std::string Value = "";
	CString(std::string value) : Value(value)
	{
		Address = MemMgr.RegionEnumerator.MemoryAlloc(ProcessInfo::hProcess, 0, Value.size() + 1);
		MemMgr.MemWriter.WriteBytes(Address, reinterpret_cast<BYTE*>(Value.data()), Value.size());
	};

	~CString()
	{
		MemMgr.RegionEnumerator.MemoryFree(ProcessInfo::hProcess, Address);
	}
};

class MonoManager
{
	MonoNativeFuncSet FunctSet;
	HANDLE hMonoModule = NULL;
	DWORD_PTR GetProcAddressFunctionAddress = 0x0;
	DWORD_PTR RootDomainAddress = 0x0;
	DWORD_PTR AttachAddress = 0x0;
	DWORD_PTR DetachAddress = 0x0;
	std::vector<DWORD_PTR> ThreadFunctionList = { 0, 0, 0 };
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
		UnityUtils.PatchAddress(Code, { 6, 16, 26, 38 }, {
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
	}






	std::vector<MonoAssemblyImage> EnumAssemblies()
	{
		std::vector<BYTE> EnumAssemblyCallback = {
			0x8b,0x02,0x3d,0xfe,0x01,0x00,0x00,0x77,0x07,0x48,0x89,0x4c,0xc2,0x08,0xff,0x02,0xc3
		};
		/*
		void _cdecl enum_assembly_callback(void* domain, CUSTOM_DOMAIN_ARRAY64 * v)
		{
			if (v->cnt <= 510) {
				v->domains[v->cnt] = (UINT64)domain;
				v->cnt += 1;
			}
		}
		*/

		// Alloc Memory
		DWORD_PTR CallBackAddress = MemMgr.RegionEnumerator.MemoryAlloc(ProcessInfo::hProcess);
		DWORD_PTR UserDataAddress = MemMgr.RegionEnumerator.MemoryAlloc(ProcessInfo::hProcess);

		// Init
		MemMgr.MemWriter.WriteBytes(CallBackAddress, EnumAssemblyCallback.data(), EnumAssemblyCallback.size());
		MemMgr.MemWriter.WriteMem<int>(UserDataAddress, 0);

		// Read Assembly List
		int AssemblyCnt = FunctSet.FunctPtrSet["mono_assembly_foreach"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, ThreadFunctionList, CallBackAddress, UserDataAddress);
		MemMgr.MemReader.ReadMem<int>(AssemblyCnt, UserDataAddress);
		std::vector<DWORD_PTR> AssemblyList = MemMgr.MemReader.ReadPointerArray(UserDataAddress + ProcessInfo::ProcOffestAdd, AssemblyCnt);

		// Create Assembly Image
		std::vector<MonoAssemblyImage> AssemblyImageList;
		for (int i = 0; i < AssemblyList.size(); i++) {
			AssemblyImageList.push_back(MonoAssemblyImage(AssemblyList[i]));
		}

		// Free Memory
		MemMgr.RegionEnumerator.MemoryFree(ProcessInfo::hProcess, UserDataAddress);
		MemMgr.RegionEnumerator.MemoryFree(ProcessInfo::hProcess, CallBackAddress);

		return AssemblyImageList;
	}

	MonoImage* FindImageByName(std::string ImageName)
	{
		std::vector<MonoAssemblyImage> Assemblies = EnumAssemblies();
		for (MonoAssemblyImage& AssemblyImage : Assemblies) {
			DWORD_PTR ImageAddress = FunctSet.FunctPtrSet["mono_assembly_get_image"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, ThreadFunctionList, AssemblyImage.Handle);
			ImageAddress &= 0xFFFFFFFFFFFF; // 12 bytes
			std::string Name = FunctSet.FunctPtrSet["mono_image_get_name"]->Call<std::string>(CALL_TYPE_CDECL, ThreadFunctionList, ImageAddress);
			std::cout << Name << std::endl;

			if(ImageName == Name)
				return new MonoImage(AssemblyImage, Name, ImageAddress);
		}
		return nullptr;
	}

	MonoClass* FindClassInImageByName(std::string ImageName, std::string ClassName)
	{
		MonoImage* Image = FindImageByName(ImageName);
		if (Image) {
			// String Preprocess
			std::string ClassNamespace = "";
			size_t dot = ClassName.rfind('.');
			if (dot != std::string::npos) {
				ClassNamespace = ClassName.substr(0, dot);
				ClassName = ClassName.substr(dot + 1);
			}

			std::ranges::replace(ClassName, '+', '/'); // 盢 '+' 蠢传 '/'

			// Get Class
			CString ClassNameObject(ClassName);
			CString ClassNamespaceObject(ClassNamespace);
			DWORD_PTR ClassAddress = 0x0;
			if (MonoNativeFuncSet::NativeFunctionExist(FunctSet, "mono_class_from_name_case"))
				ClassAddress = FunctSet.FunctPtrSet["mono_class_from_name_case"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, ThreadFunctionList, Image->Handle, ClassNamespaceObject.Address, ClassNameObject.Address);
			if(!ClassAddress)
				ClassAddress = FunctSet.FunctPtrSet["mono_class_from_name"]->Call<DWORD_PTR>(CALL_TYPE_CDECL, ThreadFunctionList, Image->Handle, ClassNamespaceObject.Address, ClassNameObject.Address);
			ClassAddress &= 0xFFFFFFFFFFFF;

			if (ClassAddress)
				return new MonoClass(Image, ClassAddress, ClassNameObject.Value, ClassNamespaceObject.Value);
			else
				return nullptr;
		}
	}


	void Test()
	{
		MonoClass* Class = FindClassInImageByName("Assembly-CSharp", "BotHandler");
		if (Class) {
			std::cout << "Class Found" << std::endl;
		}
		else {
			std::cout << "Class Not Found" << std::endl;
		}
	}
};

inline MonoManager MonoMgr = MonoManager();