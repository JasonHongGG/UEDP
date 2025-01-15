#include "BaseAddressDumper.h"
#include "../../Utils/Include/BeaEngine.h"
#include "../../Utils/Utils.h"

std::vector<DWORD_PTR> BaseAddressDumperClass::FindAddress(DWORD_PTR Address)
{
	/*
	Palworld - Win64 - Shipping.FWeakObjectPtr::operator= -40 53 - push rbx
	Palworld - Win64 - Shipping.FWeakObjectPtr::operator=+2 - 48 83 EC 20 - sub rsp, 20
	Palworld - Win64 - Shipping.FWeakObjectPtr::operator=+6 - 48 8B D9 - mov rbx, rcx
	Palworld - Win64 - Shipping.FWeakObjectPtr::operator=+9 - 48 85 D2 - test rdx, rdx
	Palworld - Win64 - Shipping.FWeakObjectPtr::operator=+C - 74 1A - je Palworld - Win64 - Shipping.FWeakObjectPtr::operator=+28
	Palworld - Win64 - Shipping.FWeakObjectPtr::operator=+E - 8B 52 0C - mov edx, [rdx + 0C]
	Palworld - Win64 - Shipping.FWeakObjectPtr::operator=+11 - 89 11 - mov[rcx], edx
	Palworld - Win64 - Shipping.FWeakObjectPtr::operator=+13 - 48 8D 0D F6EF9205 - lea rcx, [Palworld - Win64 - Shipping.GUObjectArray]					<= 找到這行，取出 "Palworld-Win64-Shipping.GUObjectArray" 表示的 Address
	Palworld - Win64 - Shipping.FWeakObjectPtr::operator=+1A - E8 D179FEFF - call Palworld - Win64 - Shipping.FUObjectArray::AllocateSerialNumber
	Palworld - Win64 - Shipping.FWeakObjectPtr::operator=+1F - 89 43 04 - mov[rbx + 04], eax
	Palworld - Win64 - Shipping.FWeakObjectPtr::operator=+22 - 48 83 C4 20 - add rsp, 20
	Palworld - Win64 - Shipping.FWeakObjectPtr::operator=+26 - 5B - pop rbx
	Palworld - Win64 - Shipping.FWeakObjectPtr::operator=+27 - C3 - ret
	*/

	std::vector<DWORD_PTR> ResultAddress;
	std::vector<std::string> Opcodes;
	std::vector<std::string> Tokens;
	Opcodes = Disassembler.BeaEngine(Address, 0x50);							// 預設大小就是 0x50 的 Offset 偏差範圍內
	if (Const::PrintAssemblyCode) Disassembler.Print(Opcodes);					// 印出組合語言的搜尋結果
	for (std::string Opcode : Opcodes) {
		Tokens.clear();
		Tokens = Utils.GetTokens(Opcode);
		//一行組合語言指令只會有一個 [ ]
		for (std::string Token : Tokens) {
			if (Token.substr(0, 3) == "[0x") {
				DWORD_PTR ptr;

				// 刪除中括號
				Token.erase(0, 1);	//刪除第一個字元
				Token.pop_back();	//刪除最後一個字元

				// String To Hex
				std::stringstream ss(Token);
				ss >> std::hex >> ptr;
				if (std::find(ResultAddress.begin(), ResultAddress.end(), ptr) == ResultAddress.end()) {
					ResultAddress.push_back(ptr);
					printf("[ 指標的 16 進位值為 ] %p\n", (void*)ptr);
				}
				break;
			}
		}
	}

	return ResultAddress;
}


bool BaseAddressDumperClass::ValidateFNamePool(DWORD_PTR Address, DWORD_PTR& FNamePoolBaseAddress, size_t& FNamePoolFirstPoolOffest)
{
	if (!Address || Address % 4 != 0) return false;

	bool FNamePoolFind_Flag = false;
	DWORD_PTR Address_Level_1 = NULL;

	// Address 是否是一個 Pointer
	if (MemMgr.MemReader.IsPointer(Address)) {

		for (int i = 0; i <= 0x10; i += 4) {
			//讀一層進去
			//printf("[ Level 1 ] %p\n", Address + i);
			if (MemMgr.MemReader.ReadMem(Address_Level_1, Address + i)) { // Address_Level_1 => NamePool 的第一層
				//printf("[ Level 2 ] %p\n", Address_Level_1);

				for (int j = 0; j < 2; j++) {
					// Address_Level_1 是否是一個 Pointer
					if (MemMgr.MemReader.IsPointer(Address_Level_1)) {
						for (int k = 0; k < 0x30; k += 2) {		//0x30 = 48		//24迴圈
							char Buffer[5];
							MemMgr.MemReader.ReadBytes(Address_Level_1 + k, (BYTE*)Buffer, 4);
							Buffer[4] = '\0';
							//printf("[ Level 3 Str ] %s\n", Buffer);
							if (strcmp(Buffer, "None") == 0) {
								//printf("[ Find NamePool !!!! ] %p\n", Address);
								printf("[ FNamePool Entry ][ i ] %X \t [ Pointer Depth ][ j ] %X \t [ First Word Offset ][ k ] %X \n", i, j, k);
								FNamePoolFind_Flag = true;
								FNamePoolBaseAddress = Address;
								FNamePoolFirstPoolOffest = i;
								break;
							}
						}
					}
					else break;

					//這邊表示是在上邊內層迴圈遞迴完後沒有找到 None 字串，因此再進去一層找(如果可以的話) => [[ Address_Level_1 ]]
					if (!MemMgr.MemReader.ReadMem(Address_Level_1, Address_Level_1)) break;
				}
			}
			else continue;
			if (FNamePoolFind_Flag) break;
		}
	}
	return FNamePoolFind_Flag;
}


void BaseAddressDumperClass::GetFNamePool()
{
	//檢查是否已經存在
	if (StorageMgr.FNamePoolBaseAddress.IsInitialized()) {
		printf("[ NamePool Exist ] %p\n\n", (void*)StorageMgr.FNamePoolBaseAddress.Get());
		return;
	}

	bool FindFlag = false;
	DWORD_PTR FNamePoolBaseAddress = NULL;
	size_t FNamePoolFirstPoolOffest = 0x0;
	DWORD_PTR tempPtr = NULL;
	std::vector<BYTE> bScan;
	std::vector<DWORD_PTR> ScanResult;
	std::vector<DWORD_PTR> FindAddressResult;

	std::vector<std::string> FNamePoolObjectAOB = {				//前兩個 UE Version 23 以下可能會沒有
		"4C 8D 05 ? ? ? ? EB 16 48 8D 0D ? ? ? ? E8"
		,"48 8D 0D ?  ?  ?  ?  E8 ?  ?  ?  ?  ?  8B ?  C6"
		,"48 83 EC 28 48 8B 05 ?  ?  ?  ?  48 85 C0 75 ?  B9 ?  ?  00 00 48 89 5C 24 20 E8"
		,"C3 ?  DB 48 89 1D ?  ?  ?  ?  ?  ?  48 8B 5C 24 20"
		,"33 F6 89 35 ? ? ? ? 8B C6 5E"
		,"8B 07 8B 0D ? ? ? ? 8B 04 81"
	};

	// Accessing elements
	for (int i = 0; i < FNamePoolObjectAOB.size(); i++) {
		printf("[ AOB ][i = % d] %s\n", i, FNamePoolObjectAOB[i].c_str());
		std::string AobStr = FNamePoolObjectAOB[i];

		//AOB Scan
		ScanResult.clear();
		bScan = MemMgr.HexStringToBytes(AobStr);
		MemMgr.Scanner.AOBSCAN(bScan.data(), bScan.size(), ScanResult);
		for (int j = 0; j < ScanResult.size(); j++) {
			printf("[ ScanResult ][j = % d] % p\n", j, (void*)ScanResult[j]);

			// 在 Function 中尋找目標 Name Pool BaseAddress
			FindAddressResult = FindAddress(ScanResult[j]);
			for (int k = 0; k < FindAddressResult.size(); k++) {
				printf("[ BaseAddressList ][k = % d] % p\n", k, (void*)FindAddressResult[k]);

				//是否是一個 Pointer
				if (MemMgr.MemReader.IsPointer(FindAddressResult[k])) {
					//驗證該 Pointer 是 NamePool BaseAddress
					FindFlag = ValidateFNamePool(FindAddressResult[k], FNamePoolBaseAddress, FNamePoolFirstPoolOffest);
					if (FindFlag) {
						StorageMgr.FNamePoolBaseAddress.Set(FNamePoolBaseAddress);
						StorageMgr.FNamePoolFirstPoolOffest.Set(FNamePoolFirstPoolOffest);
					}
				}
				if (FindFlag) break;
			}
			if (FindFlag) break;
		}
		if (FindFlag) break;
	}

	// 顯示結果
	if (FindFlag) printf("[ Find NamePool !!!! ] %p\n\n", (void*)FNamePoolBaseAddress);
	else		  printf("[ NamePool No Exist !!!! ]\n\n");
}


bool BaseAddressDumperClass::ValidateGUObjectArray(DWORD_PTR Address, DWORD_PTR& GUObjectArrayBaseAddress, size_t& GUObjectArrayElementSize)
{
	bool FindFlag = false;
	DWORD_PTR Address_Level_0 = NULL;
	DWORD_PTR Address_Level_1 = NULL;

	// Address 是否是一個 Pointer
	if (MemMgr.MemReader.IsPointer(Address)) {
		for (int i = -0x50; i <= 0x200; i += 0x4) {
			if (MemMgr.MemReader.ReadMultiLevelPointer(Address + i, 4)) {
				MemMgr.MemReader.ReadMem(Address_Level_0, Address + i);

				for (int j = 0; j < 2; j++) {
					// 找出 Array 中每個 Object 之間的 Offset (只有在 Step 剛好 10 次都落在 Object Entry 上，才會滿足第一個 if 條件)
					for (int k = 0x4; k < 0x20; k += 0x4) {		//0x30 = 48		//24迴圈		// Step
						for (int n = 0; n <= 10 * k; n += k) {									// 10 loop cnt
							if (MemMgr.MemReader.ReadMem(Address_Level_1, Address_Level_0 + n)) {

								if (
									!MemMgr.MemReader.ReadMultiLevelPointer(Address_Level_1, 3) ||
									ProcMgr.ModuleMgr.InMoudle(ProcessInfo::PID, MemMgr.MemReader.ReadMultiLevelPointer(Address_Level_1, 2)) == false ||
									DumperUtils.CheckValue<int>(Address_Level_1, 0x50, n / k, 2) == NULL			// 預設 0x50 的偏差範圍 (n / k 表示現在成功 loop 到第幾個 Object Entry) //簡單來說 Array 的 Obj 內會標示該 Obj 是第幾個，該數字要和 (n / k) 相同
									)
									break;

								if (n == (10 * k)) {		//表示迴圈完整的跑完一次
									FindFlag = true;
									printf("[ GUObjArr Entry ][ i ] %X \t [ Pointer Depth ][ j ] %X \t [ Array Group Offset ][ k ] %X \t [ Validation Cnt ][ n ] %X \n", i, j, k, n);
									GUObjectArrayBaseAddress = Address + i;
									GUObjectArrayElementSize = k;
									break;
								}
							}
						}
						if (FindFlag) break;
					}
					if (FindFlag) break;

					//再進去一層找(如果可以的話) => [ Address_Leve0 ]
					if (!MemMgr.MemReader.ReadMem(Address_Level_0, Address_Level_0)) break;
				}
			}
			if (FindFlag) break;
		}
	}

	return FindFlag;
}


void BaseAddressDumperClass::GetGUObjectArray()
{
	//檢查是否已經存在
	if (StorageMgr.GUObjectArrayBaseAddress.IsInitialized()) {
		printf("[ GUObjectArray Exist ] %p\n\n", (void*)StorageMgr.GUObjectArrayBaseAddress.Get());
		return;
	}

	bool FindFlag = false;
	DWORD_PTR GUObjectArrayBaseAddress = NULL;
	size_t GUObjectArrayElementSize = 0;
	DWORD_PTR tempPtr = NULL;
	std::vector<BYTE> bScan;
	std::vector<DWORD_PTR> ScanResult;
	std::vector<DWORD_PTR> FindAddressResult;

	std::vector<std::string> GUObjectArrayObjectAOB = {				//前兩個 UE Version 23 以下可能會沒有
		"44 8B ? ? ? 48 8D 05 ? ? ? ? ? ? ? ? ? 48 89 71 10"
		,"40 53 48 83 EC 20 48 8B D9 48 85 D2 74 ? 8B"
		,"4C 8B 05 ? ? ? ? 45 3B 88"
		,"4C 8B 44 24 60 8B 44 24 78 ? ? ? 48 8D","8B 44 24 04 56 8B F1 85 C0 74 17 8B 40 08"
		,"8B 44 24 04 56 8B F1 85 C0 74 17 8B 40 08"
		,"8B 15 ? ? ? ? 8B 04 82 85"
		,"56 48 83 ? ? 48 89 ? ? ? 48 89 ? 48 8D"
	};

	// Accessing elements
	for (int i = 0; i < GUObjectArrayObjectAOB.size(); i++) {
		printf("[ AOB ][i = % d] % s\n", i, GUObjectArrayObjectAOB[i].c_str());
		std::string AobStr = GUObjectArrayObjectAOB[i];

		//AOB Scan
		ScanResult.clear();
		bScan = MemMgr.HexStringToBytes(AobStr);
		MemMgr.Scanner.AOBSCAN(bScan.data(), bScan.size(), ScanResult);
		for (int j = 0; j < ScanResult.size(); j++) {
			printf("[ ScanResult ][j = % d] % p\n", j, (void*)ScanResult[j]);

			// 在 Function 中尋找目標 GUObjectArray BaseAddress
			FindAddressResult = FindAddress(ScanResult[j]);
			for (int k = 0; k < FindAddressResult.size(); k++) {
				printf("[ BaseAddressList ][k = % d] % p\n", k, (void*)FindAddressResult[k]);

				//是否是一個 Pointer
				if (MemMgr.MemReader.IsPointer(FindAddressResult[k])) {
					// 驗證該 Pointer 是 GUObjectArray BaseAddress
					FindFlag = ValidateGUObjectArray(FindAddressResult[k], GUObjectArrayBaseAddress, GUObjectArrayElementSize);
					if (FindFlag) {
						StorageMgr.GUObjectArrayBaseAddress.Set(GUObjectArrayBaseAddress);
						StorageMgr.GUObjectArrayElementSize.Set(GUObjectArrayElementSize);
					}
					FindFlag = true;
				}
				if (FindFlag) break;
			}
			if (FindFlag) break;
		}
		if (FindFlag) break;
	}

	// 顯示結果
	if (FindFlag) printf("[ Find GUObjectArray !!!! ] %p\n\n", (void*)GUObjectArrayBaseAddress);
	else		  printf("[ GUObjectArray No Exist !!!! ]\n\n");
}


void BaseAddressDumperClass::GetGWorld()
{
	//檢查是否已經存在
	if (StorageMgr.GWorldBaseAddress.IsInitialized()) {
		printf("[ GetGWorld Exist ] %p\n\n", (void*)StorageMgr.GWorldBaseAddress.Get());
		return;
	}

	bool FindFlag = false;
	DWORD_PTR GetGWorldBaseAddress = NULL;
	size_t GetGWorld_ArrEleSize = 0;
	DWORD_PTR tempPtr = NULL;
	std::vector<BYTE> bScan;
	std::vector<DWORD_PTR> ScanResult;
	std::vector<DWORD_PTR> FindAddressResult;

	std::vector<std::string> GUObjectArrayObjectAOB = {				//前兩個 UE Version 23 以下可能會沒有
		"48 8B 1D ? ? ? ? 48 85 DB 74 33 41 B0 01"
	};

	// Accessing elements
	for (int i = 0; i < GUObjectArrayObjectAOB.size(); i++) {
		printf("[ AOB ][i = % d] % s\n", i, GUObjectArrayObjectAOB[i].c_str());
		std::string AobStr = GUObjectArrayObjectAOB[i];

		//AOB Scan
		ScanResult.clear();
		bScan = MemMgr.HexStringToBytes(AobStr);
		MemMgr.Scanner.AOBSCAN(bScan.data(), bScan.size(), ScanResult);
		for (int j = 0; j < ScanResult.size(); j++) {
			printf("[ ScanResult ][j = % d] % p\n", j, (void*)ScanResult[j]);

			// 在 Function 中尋找目標 GetGWorld BaseAddress
			FindAddressResult = FindAddress(ScanResult[j]);
			for (int k = 0; k < FindAddressResult.size(); k++) {
				printf("[ BaseAddressList ][k = % d] % p\n", k, (void*)FindAddressResult[k]);
				FindFlag = true;
				GetGWorldBaseAddress = FindAddressResult[k];
				StorageMgr.GWorldBaseAddress.Set(GetGWorldBaseAddress);
				break;

				// 暫時不作驗證，直接取 FindAddressResult[0]
				////是否是一個 Pointer
				//ReadMemResult = MemMgr.ReadMem(FindAddressResult[k]);
				//if (ReadMemResult.second == true) {
				//	// 驗證該 Pointer 是 GetGWorld BaseAddress
				//	GetGWorldFind_Flag = ValidateGetGWorld(FindAddressResult[k], GetGWorldBaseAddress, GetGWorld_ArrEleSize);
				//	if (GetGWorldFind_Flag) {
				//		StorageMgr.SetGWorld(GetGWorldBaseAddress);
				//		StorageMgr.SetGWorld_ArrEleSize(GetGWorld_ArrEleSize);
				//	}
				//	GetGWorldFind_Flag = true;
				//}
				//if (GetGWorldFind_Flag) break;
			}
			if (FindFlag) break;
		}
		if (FindFlag) break;
	}

	// 顯示結果
	if (FindFlag) printf("[ Find GetGWorld !!!! ] %p\n\n", (void*)GetGWorldBaseAddress);
	else		  printf("[ GetGWorld No Exist !!!! ]\n\n");
}

ProcessState BaseAddressDumperClass::ShowBaseAddress()
{
	DWORD_PTR FNamePool = StorageMgr.FNamePoolBaseAddress.Get();
	if (!FNamePool) MainMenuState.GetFNamePool = ProcessState::Start;
	DWORD_PTR GUObjectArray = StorageMgr.GUObjectArrayBaseAddress.Get();
	if (!GUObjectArray) MainMenuState.GetGUObjectArray = ProcessState::Start;
	DWORD_PTR GWorld = StorageMgr.GWorldBaseAddress.Get();
	if (!GWorld) MainMenuState.GetGWorld = ProcessState::Start;
	if (!FNamePool or !GUObjectArray or !GWorld)
	{
		MainMenuState.ShowBaseAddress = ProcessState::Retry;
		return ProcessState::Retry;
	};
	printf("[ FNamePool ] %p \t [ Offset ] %llX\n", (void*)FNamePool, (size_t)FNamePool - (size_t)ProcessModule::hMainMoudle);
	printf("[ GUObject  ] %p \t [ Offset ] %llX\n", (void*)GUObjectArray, (size_t)GUObjectArray - (size_t)ProcessModule::hMainMoudle);
	printf("[ GWorld    ] %p \t [ Offset ] %llX\n", (void*)GWorld, (size_t)GWorld - (size_t)ProcessModule::hMainMoudle);
	return ProcessState::Completed;
}