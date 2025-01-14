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

struct BasicDumperObject : BasicDumperInfoObject // �l����
{
	int Bit = -1;
	std::vector<BasicDumperInfoObject> SubType;
	std::vector<std::pair<std::string, size_t>> Enum;		// �b Enum Property ���N���������o

	int Index = -1;
	std::vector<size_t> Size;			// �}�C�Ŷ����j�p => [0: �ϥΤj�p] [1: �Ŷ��j�p]
};

struct FunctionObject : BasicDumperInfoObject {
	BasicDumperObject Outer = BasicDumperObject();
	BasicDumperObject Ret = BasicDumperObject();
	std::vector<BasicDumperObject> Para;
};

struct DumperObject : BasicDumperInfoObject // �D����
{
	std::vector<BasicDumperObject> Super;
	std::vector<BasicDumperObject> Member;
	std::vector<std::pair<std::string, size_t>> Enum;		//�b Enum �����o
	std::string TypeObject = "";
	FunctionObject Funct = FunctionObject();
};

struct DumperItem : BasicDumperInfoObject {	// �ج[
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