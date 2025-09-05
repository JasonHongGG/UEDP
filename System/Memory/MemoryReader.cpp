#include "MemoryReader.h"

int MemoryReader::ReadBytes(DWORD_PTR baseAddress, BYTE* buffer, size_t size)
{
    SIZE_T bytesRead;
    if (ReadProcessMemory(ProcessInfo::hProcess, reinterpret_cast<LPCVOID>(baseAddress), buffer, size, &bytesRead))
        return bytesRead == size;
    return false;
}

DWORD_PTR MemoryReader::ReadMultiLevelPointer(DWORD_PTR baseAddress, size_t Level) {

    bool success = false;
    DWORD_PTR ptr = baseAddress;
    DWORD_PTR nextPtr = NULL;

    for (int i = 1; i <= Level; i++) {
        if (ReadMem(nextPtr, ptr)) {
            ptr = nextPtr;
            if (i == Level) success = true;
        }
        else break;
    }

    return success ? ptr : NULL;
}

bool MemoryReader::IsPointer(DWORD_PTR address)
{
    return ReadMem<DWORD_PTR>(TempDWORD, address);
}

bool MemoryReader::ReadString(DWORD_PTR baseAddress, std::string &retStr, int strMaxLen)
{
    std::unique_ptr<BYTE[]> buffer(new BYTE[strMaxLen + 1]);
    if (ReadBytes(baseAddress, buffer.get(), strMaxLen) == 0)     // 字串最長限制為 50
        return false;
    buffer[strMaxLen] = '\0';
    retStr = std::string(reinterpret_cast<char*>(buffer.get()));
    return true;
}

bool MemoryReader::ReadWString(DWORD_PTR baseAddress, std::wstring& retStr, int strMaxLen)
{
    std::unique_ptr<BYTE[]> buffer(new BYTE[strMaxLen + 1]);
    if (ReadBytes(baseAddress, buffer.get(), strMaxLen) == 0)     // 字串最長限制為 50
        return false;
    buffer[strMaxLen] = '\0';
    retStr = std::wstring(reinterpret_cast<wchar_t*>(buffer.get()));
    return true;
}
