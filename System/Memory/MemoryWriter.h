#pragma once
#include <Windows.h>
#include "../Process.h"

class MemoryWriter {
public:
    template <class T>
    bool WriteMem(DWORD_PTR baseAddress, T data);

    template <class T>
    void WriteProtectedMem(DWORD_PTR baseAddress, T data);

    bool WriteBytes(DWORD_PTR baseAddress, BYTE* data, size_t size);
private:
};


template <class T>
bool MemoryWriter::WriteMem(DWORD_PTR baseAddress, T data)
{
    return WriteProcessMemory(ProcessInfo::hProcess, (LPBYTE*)baseAddress, &data, sizeof(data), NULL);
}

template <class T>
void MemoryWriter::WriteProtectedMem(DWORD_PTR baseAddress, T data)
{
    DWORD OldProtect;
    VirtualProtectEx(ProcessInfo::hProcess, (LPVOID)baseAddress, sizeof(data), PAGE_EXECUTE_READWRITE, &OldProtect);
    WriteProcessMemory(ProcessInfo::hProcess, (LPVOID)baseAddress, &data, sizeof(data), nullptr);
    VirtualProtectEx(ProcessInfo::hProcess, (LPVOID)baseAddress, sizeof(data), OldProtect, nullptr);
}


