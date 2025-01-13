#pragma once
#include <Windows.h>
#include <string>
#include <vector>

struct BasicObjectData {
	size_t ID = -1;
	int16_t Offset = 0x0;
	std::string Type = "";
	std::string Name = "";
	std::string FullName = "";
	DWORD_PTR Address = NULL;
};

struct ObjectData : BasicObjectData {
	DWORD_PTR Outer = NULL;

	BasicObjectData ClassPtr; // ��e�ϥΪ� Class Object
	std::vector<BasicObjectData> Instance; //������ Object �|�ϥΨ�� Class Object

	BasicObjectData SuperPtr; //��e�~�Ӫ� Class Object
	std::vector<BasicObjectData> Uper; //������ Object �|�~�ө�� Class Object

	int16_t PropSize = 0;
	std::vector<BasicObjectData> Property;

	std::vector<DWORD_PTR> SubType;
	BasicObjectData TypeObjectPtr;

	size_t MemberSize = 0;
	BasicObjectData MemberPtr;

	size_t BitMask = 0;
	DWORD_PTR Funct = 0x0;
};

