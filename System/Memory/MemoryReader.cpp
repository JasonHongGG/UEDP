#include "MemoryReader.h"


template <class T>
T MemoryReader::ReadMem(DWORD_PTR baseAddress, size_t size)
{
    T x;
    if (size == -1) size = sizeof(x);
    if (ReadProcessMemory(ProcessInfo::hProcess, reinterpret_cast<LPCVOID>(baseAddress), &x, size, NULL) == FALSE) return T(); // 回傳預設值和失敗標誌
    return x;
}

bool MemoryReader::ReadBytes(DWORD_PTR baseAddress, BYTE* buffer, size_t size)
{
    SIZE_T bytesRead;
    if (ReadProcessMemory(ProcessInfo::hProcess, reinterpret_cast<LPCVOID>(baseAddress), buffer, size, &bytesRead))
        return bytesRead == size;
    return false;
}

DWORD_PTR MemoryReader::ReadMultiLevelPointer(DWORD_PTR baseAddress, size_t Level) {

    bool success = false;
    DWORD_PTR ptr = baseAddress;
    DWORD_PTR nextPtr;

    for (int i = 1; i <= Level; i++) {
        nextPtr = ReadMem<DWORD_PTR>(ptr);
        if (nextPtr) {
            ptr = nextPtr;
            if (i == Level) success = true;
        }
        else break;
    }

    return success ? ptr : NULL;
}
