#pragma once
#include "../../imgui/imgui.h"
#include <Windows.h>
#include <string>
#include <vector>
#include <map>

struct BasicDumperInfoObject
{
	size_t ID = 0;
	size_t  Offset = 0x0;
	std::string Type = "";
	std::string Name = "";
	std::string FullName = "";
	DWORD_PTR ObjectAddress = 0x0;
	DWORD_PTR Address = 0x0;
	size_t  Value = 0;
	bool Clickable = false;
};

struct BasicDumperObject : BasicDumperInfoObject // 子物件
{
	int Bit = -1;
	std::vector<BasicDumperInfoObject> SubType;
	std::vector<std::pair<std::string, size_t>> Enum;		// 在 Enum Property 中就間接先取得

	int Index = -1;
	std::vector<size_t> Size;			// 陣列空間的大小 => [0: 使用大小] [1: 空間大小]
};

struct FunctionObject : BasicDumperInfoObject {
	BasicDumperObject Outer = BasicDumperObject();
	BasicDumperObject Ret = BasicDumperObject();
	std::vector<BasicDumperObject> Para;
};

struct DumperObject : BasicDumperInfoObject // 主物件
{
	std::vector<BasicDumperObject> Super;
	std::vector<BasicDumperObject> Member;
	std::vector<std::pair<std::string, size_t>> Enum;		//在 Enum 中取得
	std::string TypeObject = "";
	FunctionObject Funct = FunctionObject();
};

struct DumperItem : BasicDumperInfoObject {	// 框架
	bool IsExist = true;
	int Index = -1;
	ImVector<int> SelectedList;
	std::map<DWORD_PTR, DumperObject> ObjectMap;
};





struct FNameInfoStruct {
	std::string InputStr = "";
	std::string FName = "[Result][ Get FName By ID  ]";
};

struct GUObjectInfoStruct {
	// Option
	std::vector<std::string> ModeList = { "Address", "ID", "Name" };
	int ModeListSelectIndex = 0;

	// Panel
	std::string InputStr = "";
	std::string ID = "[Result][ ObjectID  ]";
	std::string Type = "[Result][ Type  ]";
	std::string Name = "[Result][ Name  ]";
	std::string FullName = "[Result][ FullName  ]";
	std::string Address = "[Result][ Address  ]";
	std::string Offset = "[Result][ Offset  ]";
	std::string ClassPtr = "[Result][ ClassPtr  ]";
	std::string Funct = "[Result][ Funct  ]";
	std::string Outer = "[Result][ Outer  ]";
	std::string SuperPtr = "[Result][ SuperPtr  ]";
	std::string PropSize = "[Result][ Propsize  ]";
	std::string PropPtr_0 = "[Result][ Property 1  ]";
	std::string	PropPtr_1 = "[Result][ Property 2  ]";
	std::string MemberSize = "[Result][ MemberSize  ]";
	std::string MemberPtr_0 = "[Result][ Member 0 ]";
	std::string Bitmask = "[Result][ BitMask  ]";
};