#pragma once
#pragma warning (disable : 6001 )
#pragma warning (disable : 4244 )

#define WIN32_LEAN_AND_MEAN 

#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <mutex>

#include "Process.h"
#include "../Utils/Utils.h"
#include "Memory/MemoryReader.h"
#include "Memory/MemoryWriter.h"
#include "Memory/MemorySearch.h"    
#include "Memory/MemoryRegionEnumerator.h"






class Memory {
public:
    MemoryReader MemReader;
    MemoryWriter MemWriter;
    MemorySearch Scanner;
    EnumProcessMemoryRegions RegionEnumerator;

    Memory(){
        MemReader = MemoryReader();
        MemWriter = MemoryWriter();
        Scanner = MemorySearch();
        RegionEnumerator = EnumProcessMemoryRegions();
    }
    ~Memory(){}

    void AddressToBytes(DWORD_PTR Address, BYTE BytesArray[]);

    std::vector<BYTE> HexStringToBytes(const std::string& hex);
};

inline Memory MemMgr = Memory();

class MemmoryStorageManager
{
public:
    // ==============================================================================================
    // Memory Search
    // Temp Save AOBSCAN Result (Prevent Race Condition)
    std::vector<DWORD_PTR> AOBSCAN_Ret;
    std::mutex AOBSCAN_Mtx;
    void AOBSCAN_Ret_PushBack(DWORD_PTR Address) {
        std::lock_guard<std::mutex> lock(AOBSCAN_Mtx);
        AOBSCAN_Ret.push_back(Address);
    }
    void AOBSCAN_Ret_Clear() {
        AOBSCAN_Ret.clear();
    }
    std::vector<DWORD_PTR> GetAOBSCAN_Ret() {
        return AOBSCAN_Ret;
    }

private:
};
inline MemmoryStorageManager MemStoreMgr = MemmoryStorageManager();


