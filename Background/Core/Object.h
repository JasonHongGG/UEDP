#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include "UEOffset.h"
#include "Parser.h"
#include "../Interface/ObjectInterface.h"
#include "../../System/Memory.h"
#include "../../Utils/Include/BeaEngine.h"
#include "../../Utils/Utils.h"

class ObjectManager
{
public:


	bool BasicObjectDataWapper(ObjectData& PropertyObject, BasicObjectData& TempBasicObjectData);

	bool GetBasicInfo_1(DWORD_PTR Address, size_t& ObjectId, std::string& Name, std::string& Type, DWORD_PTR& Outer);
	bool GetBasicInfo_2(DWORD_PTR Address, size_t& ObjectId, std::string& Name, std::string& Type, DWORD_PTR& Class, DWORD_PTR& Outer);
	bool GetBasicInfo(ObjectData& RetObjectData, DWORD_PTR Address, size_t Level, DWORD_PTR ClassPtr);

	void GetFullName(ObjectData& RetObjectData, DWORD_PTR Address, size_t Level);

	bool PropertyProcess(ObjectData& RetObjectData, DWORD_PTR Address, size_t Level);

	void GetProperty(ObjectData& RetObjectData, DWORD_PTR Address, size_t Level);

	void GetMember(ObjectData& RetObjectData, DWORD_PTR Address, size_t Level);

	void GetFunction(ObjectData& RetObjectData, DWORD_PTR Address, size_t Level);

	std::string ExtractPackageName(const std::string& input);
	void SetPackage(ObjectData& RetObjectData);

	/*
	Address : Object �� Address
	ObjData : �Ψӱ��� Object Data ���ѼơA�b TrySaveObject �L�{���|�v���N�ѪR����Ʃ�J
	Level : ��j�����j�h��
	(�򥻤W�W�L 50 �N�|�z�F)
	SkipGetFullName : �O�_���L FullName ���R�A�D�n�O Member Object �|�ݭn���L���R�A�]���Y�}�o�̳]�p����o�����i��|�X�{�L���j��
	(�A�� FullName �O�A�ۤv������)
	SearchMode : �O�_�u�O��s�b�j�M
	( ���ѼƥD�n�O�b Parse �e�|�ϥΡAEX: AutoConfig �b�]�w�ѼƮɥi��N�|�H�Y�� Object �ӳ]�w�A
	  ���O�ѩ�Ѽ��٤��ǽT�A�]����e���R�����G�|�˱�A�ҥH�����槹��ѪR�A������������ )
	*/
	bool TrySaveObject(DWORD_PTR Address, ObjectData& ObjData, size_t Level, bool SkipGetFullName = false, bool SearchMode = false);

private:

public:
	size_t MaxLevel = 100;

};

inline ObjectManager ObjectMgr = ObjectManager();