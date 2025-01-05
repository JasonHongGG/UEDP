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