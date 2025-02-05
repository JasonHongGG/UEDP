#pragma once
#include <vector>
#include <Windows.h>
#include "MonoConst.h"
#include "../../System/Memory.h"

class MonoUtilsSet
{
public:
	void PatchAddress(std::vector<BYTE>& Code, std::vector<int> Offsets, std::vector<DWORD_PTR> Addresses) {
		for (int i = 0; i < Offsets.size(); i++) {
			int Offset = Offsets[i];
			DWORD_PTR Address = Addresses[i];

			if (Offset + 8 > Code.size()) {
				std::cerr << "Offset out of range!" << std::endl;
				return;
			}
			std::memcpy(&Code[Offset], &Address, sizeof(DWORD_PTR));
		}
	}

	template <typename T>
	T ReadValue(ValueTypeEnum ReturnType, DWORD_PTR Address)
	{
		T result;
		switch (ReturnType) {
		case TYPE_BOOL:
		case TYPE_CHAR:
		case TYPE_UCHAR:
		case TYPE_INT16:
		case TYPE_UINT16:
		case TYPE_INT32:
		case TYPE_UINT32:
		case TYPE_INT64:
		case TYPE_UINT64:
		case TYPE_FLOAT:
		case TYPE_DOUBLE:
		case TYPE_VOID_P:
			MemMgr.MemReader.ReadMem(result, Address);
			break;
		case TYPE_CHAR_P: {
			if constexpr (std::is_same<T, std::string>::value) {
				BYTE Buffer[60];
				DWORD_PTR StrAddress;
				MemMgr.MemReader.ReadMem(StrAddress, Address);
				MemMgr.MemReader.ReadString(StrAddress, Buffer);
				result = std::string(reinterpret_cast<char*>(Buffer));
			}
			break;
		}
		default:
			if constexpr (std::is_same_v<T, std::string>)
				result = "";  // 對於 std::string，返回空字串
			else
				result = static_cast<T>(0);  // 其他數值型態，返回 0

			break;
		}

		return result;
	}
};
inline MonoUtilsSet MonoUtils = MonoUtilsSet();


class CString
{
public:
	DWORD_PTR Address = 0x0;
	std::string Value = "";
	CString(std::string value) : Value(value)
	{
		Address = MemMgr.RegionEnumerator.MemoryAlloc(ProcessInfo::hProcess, 0, Value.size() + 1);
		MemMgr.MemWriter.WriteBytes(Address, reinterpret_cast<BYTE*>(Value.data()), Value.size());
	};

	~CString()
	{
		MemMgr.RegionEnumerator.MemoryFree(ProcessInfo::hProcess, Address);
	}
};