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

private:
    void PrintContent(bool prologue = false, MEMORY_BASIC_INFORMATION mbi = MEMORY_BASIC_INFORMATION());
};