#pragma once
#include <Windows.h>

class MemoryWriter {
public:
    template <class T>
    void WriteMem(DWORD_PTR baseAddress, T data);

    template <class T>
    void WriteProtectedMem(DWORD_PTR baseAddress, T data);

    template <class T> // 也可用 template <typename T> 是相同的
    void WriteBytes(DWORD baseAddress, T data, size_t size);
private:
};