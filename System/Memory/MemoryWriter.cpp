#include "MemoryWriter.h"

template <class T>
void MemoryWriter::WriteMem(DWORD_PTR baseAddress, T data)
{
    WriteProcessMemory(ProcessInfo::hProcess, (LPBYTE*)baseAddress, &data, sizeof(data), NULL);
}

template <class T>
void MemoryWriter::WriteProtectedMem(DWORD_PTR baseAddress, T data)
{
    DWORD OldProtect;
    VirtualProtectEx(ProcessInfo::hProcess, (LPVOID)baseAddress, sizeof(data), PAGE_EXECUTE_READWRITE, &OldProtect);
    WriteProcessMemory(ProcessInfo::hProcess, (LPVOID)baseAddress, &data, sizeof(data), nullptr);
    VirtualProtectEx(ProcessInfo::hProcess, (LPVOID)baseAddress, sizeof(data), OldProtect, nullptr);
}

template <class T>
void MemoryWriter::WriteBytes(DWORD baseAddress, T data, size_t size)
{
    DWORD OldProtect;
    VirtualProtectEx(ProcessInfo::hProcess, (LPVOID)baseAddress, size, PAGE_READWRITE, &OldProtect);
    WriteProcessMemory(ProcessInfo::hProcess, (LPVOID)baseAddress, data, size, 0);
    VirtualProtectEx(ProcessInfo::hProcess, (LPVOID)baseAddress, size, OldProtect, nullptr);
}