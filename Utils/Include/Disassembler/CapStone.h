#pragma once

#include <vector>
#include <string>
#include <Windows.h>
#include "../../../System/Memory.h"
extern "C" {
#include "../CapStone/capstone.h"
#pragma comment(lib,"Utils/Include/CapStone/capstone.lib")
}
#include "IDisassembler.h"

class CapStoneDisassembler : public IDisassembler
{
public:
    CapStoneDisassembler() : handle(0) {
        // Initialize Capstone
        if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) {
            printf("Failed to initialize Capstone\n");
        }
        cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
    }

    ~CapStoneDisassembler() {
        // Clean up Capstone handle
        if (handle) {
            cs_close(&handle);
        }
    }

    std::vector<std::string> Disassemble(uint64_t& address, int size = 0x200, bool PrintMode = false) {
        std::vector<std::string> Opcodes;

        BYTE* pBuffer = new BYTE[size];
        BYTE wirteByteNum = MemMgr.MemReader.ReadBytes(address, pBuffer, size);
        DisassembleByBytes(address, pBuffer, size, Opcodes, PrintMode);
        delete[] pBuffer;
        return Opcodes;
    }

    void DisassembleByBytes(uint64_t baseAddress, BYTE* BufferAddress, int size, std::vector<std::string>& Opcodes, bool PrintMode)
    {
        cs_insn* insn;
        size_t count;
		size_t ripOffset = 0;

        // Disassemble the bytes using Capstone
        count = cs_disasm(handle, BufferAddress, size, baseAddress, 0, &insn);
        if (count > 0) {
            for (size_t i = 0; i < count; ++i) {
				bool trigger = false;
                std::string instruction = std::string(insn[i].mnemonic);
                std::string originalOpStr = std::string(insn[i].op_str);
                ripOffset += insn[i].size;

                // Check if the instruction has operands with detailed info
                if (insn[i].detail != nullptr) {
                    for (int j = 0; j < insn[i].detail->x86.op_count; ++j) {
                        cs_x86_op& op = insn[i].detail->x86.operands[j];
                        
                        // If the operand is a RIP-relative address, calculate the effective address
                        if (op.type == X86_OP_MEM && op.mem.segment == 0 && op.mem.base == X86_REG_RIP) {
                            trigger = true;
                            // Calculate the effective address by adding the offset to the RIP base
                            
                            uint64_t effective_address = baseAddress + op.mem.disp + ripOffset;
                            char addr_str[64];
                            snprintf(addr_str, sizeof(addr_str), "[0x%016llx]", effective_address);

                            // Remove the old RIP-relative portion (e.g., [rip + 0x99C0])
                            size_t pos = originalOpStr.find("[rip");
                            if (pos != std::string::npos) {
                                size_t endPos = originalOpStr.find("]", pos);
                                if (endPos != std::string::npos) {
                                    originalOpStr.erase(pos, endPos - pos + 1);  // Remove [rip + offset]
                                }
                            }

                            originalOpStr.insert(pos, addr_str);
                            instruction += " " + originalOpStr;
                        }
                    }
                }

                if (!trigger) {
                    instruction = std::string(insn[i].mnemonic) + " " + std::string(insn[i].op_str);
                }

                Opcodes.push_back(instruction);
                if (PrintMode) {
                    printf("%s\n", instruction.c_str());
                }
            }
            cs_free(insn, count);
        }
        else {
            if (PrintMode) {
                printf("[ Disassembly Error ] => Failed to disassemble bytes\n");
            }
        }
    }


    void Print(std::vector<std::string> Opcodes)
    {
        printf("[ Disassemble Code ]\n");
        for (int i = 0; i < Opcodes.size(); i++) {
            printf("[ %d Inc ] %s\n", i + 1, Opcodes[i].c_str());
        }
    }

private:
    csh handle;  // Capstone handle
};
