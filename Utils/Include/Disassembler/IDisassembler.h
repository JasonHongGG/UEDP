#pragma once
#include <vector>
#include <string>
#include <Windows.h>

class IDisassembler
{
public:
    virtual std::vector<std::string> Disassemble(uint64_t& address, int size = 0x200, bool PrintMode = false) = 0;

    virtual void DisassembleByBytes(uint64_t baseAddress, BYTE* BufferAddress, int size, std::vector<std::string>& Opcodes, bool PrintMode) = 0;

    virtual void Print(std::vector<std::string> Opcodes) = 0;
};