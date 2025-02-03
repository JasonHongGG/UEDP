#pragma once
#include <Windows.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <TlHelp32.h>
#include <tchar.h>

class EnumProcessMemoryRegions
{
public:
    EnumProcessMemoryRegions() {};
    ~EnumProcessMemoryRegions() {};

    std::vector<MEMORY_BASIC_INFORMATION> enumMemoryRegions(HANDLE hProc, bool printMode = false);

    // 根據 Address 找到該塊 malloc 的 memory region
    bool GetMemoryRegionSizeByAddress(HANDLE hProc, DWORD_PTR Address, size_t& Size, bool printMode = false);

    uint64_t GetMostNearSpaceByAddress(std::vector<MEMORY_BASIC_INFORMATION> regions, uint64_t TargetAddress, uint64_t requiredSize);

    void WriteMemoryRegionsToFile(const std::vector<MEMORY_BASIC_INFORMATION>& regions, const std::string& filename);

    DWORD_PTR MemoryAlloc(HANDLE procHanlder, DWORD_PTR Address = NULL, size_t size = 4096, size_t type = MEM_COMMIT | MEM_RESERVE, size_t protect = PAGE_EXECUTE_READWRITE)
    {
        LPVOID pMemory = VirtualAllocEx(
                            procHanlder, // 當前進程
                            (LPVOID)Address,                // 讓系統選擇地址
                            size,                // 內存大小
                            type, // 分配並提交
                            protect       // 可讀寫
                        );

        return (DWORD_PTR)pMemory;
    }

	void MemoryFree(HANDLE procHanlder, DWORD_PTR Address)
	{
		VirtualFreeEx(procHanlder, LPVOID(Address), 0, MEM_RELEASE);
	}

private:
    void PrintContent(bool prologue = false, MEMORY_BASIC_INFORMATION mbi = MEMORY_BASIC_INFORMATION());
};