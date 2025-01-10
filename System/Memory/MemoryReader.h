#pragma once
#include <Windows.h>
#include "../Process.h"

class MemoryReader {
public:
    template <class T>
    T ReadMem(DWORD_PTR baseAddress, size_t size = -1);

    bool ReadBytes(DWORD_PTR baseAddress, BYTE* buffer, size_t size);

    DWORD_PTR ReadMultiLevelPointer(DWORD_PTR baseAddress, size_t Level);
private:
};

template <class T>
T MemoryReader::ReadMem(DWORD_PTR baseAddress, size_t size)
{
    T x;
    if (size == -1) size = sizeof(x);
    if (ReadProcessMemory(ProcessInfo::hProcess, reinterpret_cast<LPCVOID>(baseAddress), &x, size, NULL) == FALSE) return T(); // 回傳預設值和失敗標誌
    return x;
}