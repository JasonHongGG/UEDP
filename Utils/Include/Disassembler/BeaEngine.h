﻿#pragma once

#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include "../../../System/Memory.h"
#define BEA_ENGINE_STATIC  // 指明使用静态Lib库
#define BEA_USE_STDCALL    // 指明使用stdcall调用约定
extern "C" {
#include "../Beaengine/BeaEngine.h"
#pragma comment(lib, "Utils/Include/BeaEngine/BeaEngine_s_d_l_stdcall_64.lib")
}
#include "IDisassembler.h"


/* 
所有參數說明 DOC: https://github.com/BeaEngine/beaengine/blob/master/doc/beaengine.md
*/

class BeaEngineDisassembler : public IDisassembler
{
public:
	BeaEngineDisassembler() {};
	~BeaEngineDisassembler() {};

	std::vector<std::string> Disassemble(uint64_t& address, int size = 0x200, bool PrintMode = false) {
		std::vector<std::string> Opcodes;
		BYTE* pBuffer = new BYTE[size];
		BYTE wirteByteNum = MemMgr.MemReader.ReadBytes(address, pBuffer, size);		
		DisassembleByBytes(address, pBuffer, size, Opcodes, PrintMode);
		delete[] pBuffer;	
		return Opcodes;

	}

	void DisassembleByBytes(uint64_t baseAddress, BYTE* BufferAddress, int size, std::vector<std::string> &Opcodes, bool PrintMode)
	{
		
		int len;
		std::string str;
		uint64_t BufferEndAddress = (uint64_t)BufferAddress + size;

		DISASM Disasm_Info;
		(void)memset(&Disasm_Info, 0, sizeof(DISASM));
		Disasm_Info.EIP = (UInt64)BufferAddress;
		Disasm_Info.VirtualAddr = baseAddress;
		Disasm_Info.Archi = 64;
		Disasm_Info.Options = NasmSyntax | PrefixedNumeral ;	//ShowSegmentRegs
		while (!Disasm_Info.Error)
		{
			Disasm_Info.SecurityBlock = BufferEndAddress - Disasm_Info.EIP;
			if (Disasm_Info.SecurityBlock <= 0) { if(PrintMode) printf("[Disassemble Completed] => Normal Exit \n"); break; }
				
			len = Disasm(&Disasm_Info);
			switch (Disasm_Info.Error)	//指令如果不能完整讀取出來 => 指令不完全 => Disasm_Info.Error 會為 -2(OUT_OF_BLOCK)
			{
			case OUT_OF_BLOCK:
				if (PrintMode) printf("[ OUT_OF_BLOCK ] => Error\n");
				break;
			case UNKNOWN_OPCODE:
				if (PrintMode) printf("%s \n", Disasm_Info.CompleteInstr);
				str.assign(Disasm_Info.CompleteInstr, strlen(Disasm_Info.CompleteInstr));
				Opcodes.push_back(str);
				Disasm_Info.EIP += 1; //略過一個 byte
				Disasm_Info.EIP += 1;
				Disasm_Info.Error = 0; //恢復 ERROR 狀態
				break;
			default:
				if (PrintMode) printf("%s \n", Disasm_Info.CompleteInstr);
				str.assign(Disasm_Info.CompleteInstr, strlen(Disasm_Info.CompleteInstr));
				Opcodes.push_back(str);
				Disasm_Info.EIP += len;
				Disasm_Info.VirtualAddr += len;
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
};

