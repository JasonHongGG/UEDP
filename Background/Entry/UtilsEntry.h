#pragma once
#include <Windows.h>
#include <vector>
#include "../Interface/ObjectInterface.h"
#include "../../GUI/Interface/DumperConsoleInterface.h"
#include "../../GUI/Config/MainConsoleConfig.h"
#include "../../State/GlobalState.h"
#include "../../System/Memory.h"
#include "../Core/UEOffset.h"
#include "../Core/Parser/FNamePoolParser.h"
#include "../Storage/StorageManager.h"

namespace UtilsEntry
{
	inline bool IsObjectNameInPackage(std::string TargetObjectName, DWORD_PTR& Address);

	inline size_t CalcuTypeOffset(std::string Type);


	inline DWORD_PTR PropertySelector(ObjectData& Object);


	inline bool DumperObjectWapper(ObjectData& Object, BasicDumperObject& TempBasicDumperObject);


	inline bool DumperObjectBasicWapper(BasicObjectData& BasicObject, BasicDumperObject& TempBasicDumperObject);


	inline void InspectorAdditionalObjProc(ObjectData& Object, BasicDumperObject& TempBasicObject, int LoopIndex, size_t Offset);


	inline bool ObjectSubTypeProc(DWORD_PTR Address, BasicDumperObject& TempBasicObject, bool CheckClickable = false);


	inline bool GetEnumList(DWORD_PTR Address, std::vector<std::pair<std::string, size_t>>& EnumList);


	inline void ObjectEnumProc(ObjectData& EnumObject, BasicDumperObject& TempBasicObject, bool IsPropertyObject = true);


	inline void ObjectMemberProcessSelector(DWORD_PTR Address, std::vector<BasicDumperObject>& MemberList, bool Preview = false, ProcessState* ProcessState = NULL, DWORD_PTR* ProcessAddress = NULL);


	inline bool ObjectMemberProc(ObjectData& RetObjData, std::vector<BasicDumperObject>& MemberList, bool IsObject, bool Preview = false, ProcessState* ProcessState = NULL, DWORD_PTR* ProcessAddress = NULL);


	inline ProcessState ObjectMemberListProc(ProcessClass ProcessClass, ObjectData& RetObjData, DumperObject& TempObjectItem, bool Preview = false, ProcessState* ProcessState = NULL, DWORD_PTR* ProcessAddress = NULL);

}


bool UtilsEntry::IsObjectNameInPackage(std::string TargetObjectName, DWORD_PTR& Address) {
	bool found = false;

	// �b�h�h map ���M��S�w�� ObjectName
	for (auto& NamesEntry : StorageMgr.GetPackage()) {
		for (auto& TypeEntry : NamesEntry.second) {
			for (auto& ObjectEntry : TypeEntry.second) {
				std::string ObjectName = ObjectEntry.second.first;
				if (ObjectName == TargetObjectName) {
					found = true;
					Address = ObjectEntry.second.second;
					break; // �Y���ؼ� ObjectName�A�h�����j��
				}
			}
			if (found) break;
		}
		if (found) break;
	}
	return found;
}

size_t UtilsEntry::CalcuTypeOffset(std::string Type)
{
	if (Type.find("Int64Property") != std::string::npos or
		Type.find("UInt64Property") != std::string::npos or
		Type.find("DoubleProperty") != std::string::npos or
		Type.find("MapProperty") != std::string::npos or
		Type.find("ArrayProperty") != std::string::npos or
		Type.find("StructProperty") != std::string::npos or
		Type.find("ClassProperty") != std::string::npos or
		Type.find("ClassPtrProperty") != std::string::npos or
		Type.find("ObjectProperty") != std::string::npos or
		Type.find("EnumProperty") != std::string::npos or
		Type.find("StrProperty") != std::string::npos or
		Type.find("TextProperty") != std::string::npos or
		Type.find("NameProperty") != std::string::npos
		)
	{
		return 0x8;
	}
	else if (Type.find("IntProperty") != std::string::npos or
		Type.find("Int32Property") != std::string::npos or
		Type.find("UIntProperty") != std::string::npos or
		Type.find("UInt32Property") != std::string::npos or
		Type.find("FloatProperty") != std::string::npos
		)
	{
		return 0x4;
	}
	else if (Type.find("Int16Property") != std::string::npos or
		Type.find("UInt16Property") != std::string::npos
		)
	{
		return 0x2;
	}
	else if (Type.find("ByteProperty") != std::string::npos or
		Type.find("Int8Property") != std::string::npos or
		Type.find("UInt8Property") != std::string::npos or
		Type.find("BoolProperty") != std::string::npos		//<= bool �����ˬd
		)
	{
		return 0x1;
	}

	return 0x0; //�ثe�٤��䴩�����󫬺A�������L
}

DWORD_PTR UtilsEntry::PropertySelector(ObjectData& Object) {
	return (Object.Property.size() >= 2) ? Object.Property[1].Address : Object.Property[0].Address;
}

bool UtilsEntry::DumperObjectWapper(ObjectData& Object, BasicDumperObject& TempBasicDumperObject)
{
	TempBasicDumperObject.Name = Object.Name;
	TempBasicDumperObject.Type = Object.Type;
	TempBasicDumperObject.Offset = Object.Offset;
	TempBasicDumperObject.Address = Object.Address;
	TempBasicDumperObject.Value = Object.Address;
	TempBasicDumperObject.ObjectAddress = Object.Address;
	TempBasicDumperObject.ID = Object.ID;
	return true;
}


bool UtilsEntry::DumperObjectBasicWapper(BasicObjectData& BasicObject, BasicDumperObject& TempBasicDumperObject)
{
	TempBasicDumperObject.Name = BasicObject.Name;
	TempBasicDumperObject.Type = BasicObject.Type;
	TempBasicDumperObject.Offset = BasicObject.Offset;
	TempBasicDumperObject.Address = BasicObject.Address;
	TempBasicDumperObject.Value = BasicObject.Address;
	TempBasicDumperObject.ObjectAddress = BasicObject.Address;
	TempBasicDumperObject.ID = BasicObject.ID;
	return true;
}

void UtilsEntry::InspectorAdditionalObjProc(ObjectData& Object, BasicDumperObject& TempBasicObject, int LoopIndex, size_t Offset)
{
	// Warpper Basic Infomation
	DumperObjectWapper(Object, TempBasicObject);

	// SubType
	ObjectSubTypeProc(Object.Address, TempBasicObject);

	// Enum
	if (Object.Type.find("EnumProperty") != std::string::npos)
		ObjectEnumProc(Object, TempBasicObject);

	TempBasicObject.Index = LoopIndex;
	TempBasicObject.Offset = Offset;
}

bool UtilsEntry::ObjectSubTypeProc(DWORD_PTR Address, BasicDumperObject& TempBasicObject, bool CheckClickable)
{
	// �D�n�O Array ���U�i��O��L Struct
	DWORD_PTR TempAddress;
	ObjectData TempObjData;
	ObjectData SubTypeObjData;
	BasicDumperInfoObject TempSubTypeObject;
	StorageMgr.GetObjectDict(Address, SubTypeObjData);
	if (!SubTypeObjData.SubType.empty()) {
		for (int j = 0; j < SubTypeObjData.SubType.size(); j++) {
			StorageMgr.GetObjectDict(SubTypeObjData.SubType[j], TempObjData);
			TempSubTypeObject.Name = TempObjData.Name;
			TempSubTypeObject.Type = TempObjData.Type;
			TempSubTypeObject.Address = TempObjData.Address;
			TempSubTypeObject.Clickable = (CheckClickable ? IsObjectNameInPackage(TempObjData.Name, TempAddress) : false);
			TempBasicObject.SubType.push_back(TempSubTypeObject);
		}
	}
	return true;
}

bool UtilsEntry::GetEnumList(DWORD_PTR Address, std::vector<std::pair<std::string, size_t>>& EnumList)
{
	DWORD_PTR Address_Level_1;
	std::string EnumName;
	size_t EnumIndex, EnumFNameIndex;		// EnumIndex �n�����|�Ψ�A���� Emun �j�h���O�q 0 �}�l
	DWORD_PTR EnumListAddress = MemMgr.MemReader.ReadMem<DWORD_PTR>(Address + UEOffset.EnumList);
	int EnumListSize = MemMgr.MemReader.ReadMem<int>(Address + UEOffset.EnumList + ProcessInfo::ProcOffestAdd);
	for (int i = 0; i < EnumListSize; i++) {
		Address_Level_1 = EnumListAddress + i * UEOffset.EnumPropMul;
		EnumFNameIndex = MemMgr.MemReader.ReadMem<int>(Address_Level_1 + UEOffset.EnumPropName);
		FNameParser.GetFNameStringByID(EnumFNameIndex, EnumName, true);
		EnumIndex = MemMgr.MemReader.ReadMem<int>(Address_Level_1 + UEOffset.EnumPropIndex);
		EnumList.push_back(std::make_pair(EnumName, EnumIndex));
	}

	return true;
}


void UtilsEntry::ObjectEnumProc(ObjectData& EnumObject, BasicDumperObject& TempBasicObject, bool IsPropertyObject)
{
	ObjectData TempObjData;
	if (!IsPropertyObject) TempObjData = EnumObject;
	else if (!StorageMgr.GetObjectDict(PropertySelector(EnumObject), TempObjData, true)) return;
	std::vector<std::pair<std::string, size_t>> TempEnumList;
	GetEnumList(TempObjData.Address, TempEnumList);
	TempBasicObject.Enum = TempEnumList;
}


void UtilsEntry::ObjectMemberProcessSelector(DWORD_PTR Address, std::vector<BasicDumperObject>& MemberList, bool Preview, ProcessState* ProcessState, DWORD_PTR* ProcessAddress)
{
	if (!Preview) {
		ObjectData ObjData;
		BasicDumperObject BasicObject;
		StorageMgr.GetObjectDict(Address, ObjData);
		DumperObjectWapper(ObjData, BasicObject);
		MemberList.push_back(BasicObject);
	}
	else {
		// Preview => �]�w�����Ѽƫ��^�A���U�@�^�X
		*(ProcessState) = ProcessState::Retry;
		*(ProcessAddress) = Address;
	}
}


bool UtilsEntry::ObjectMemberProc(ObjectData& RetObjData, std::vector<BasicDumperObject>& MemberList, bool IsObject, bool Preview, ProcessState* ProcessState, DWORD_PTR* ProcessAddress)
{
	DWORD_PTR Address_Level_1 = NULL;
	ObjectData TempObjData;

	if (!RetObjData.Property.empty())
		ObjectMemberProcessSelector(PropertySelector(RetObjData), MemberList, Preview, ProcessState, ProcessAddress);

	// �_�h�N�� TypeObject
	else {
		Address_Level_1 = MemMgr.MemReader.ReadMem<DWORD_PTR>(RetObjData.Address + UEOffset.TypeObject);
		StorageMgr.GetObjectDict(Address_Level_1, TempObjData);

		// �p�G�� Member �h�����^�ǸӪ���  || �p�G�O�w�g�O Object 
		if (IsObject or TempObjData.MemberPtr.Address != NULL)
			ObjectMemberProcessSelector(TempObjData.Address, MemberList, Preview, ProcessState, ProcessAddress);
		// �p�G�� Property
		else if (TempObjData.Property.empty())
			ObjectMemberProcessSelector(TempObjData.Property[0].Address, MemberList, Preview, ProcessState, ProcessAddress);
		// �p�G�S�� Property => �� super
		else if (StorageMgr.GetObjectDict(TempObjData.SuperPtr.Address, TempObjData))	// ������ SuperPtr.Address ����
			ObjectMemberProcessSelector(TempObjData.SuperPtr.Address, MemberList, Preview, ProcessState, ProcessAddress);
		else
			return false;
	}
	return true;
}


ProcessState UtilsEntry::ObjectMemberListProc(ProcessClass ProcessClass, ObjectData& RetObjData, DumperObject& TempObjectItem, bool Preview, ProcessState* ProcessState, DWORD_PTR* ProcessAddress)
{
	DWORD_PTR Address_Level_1 = NULL;
	ObjectData TempObjData;
	BasicDumperObject TempBasicObject;
	std::vector<BasicDumperObject> TempMemberList;

	// Basic Data
	TempObjectItem.ObjectAddress = RetObjData.Address;
	TempObjectItem.Type = RetObjData.Type;
	TempObjectItem.Name = RetObjData.Name;
	TempObjectItem.Offset = RetObjData.Offset;
	TempObjectItem.Address = RetObjData.Address;		//�ݧ��
	TempObjectItem.Value = RetObjData.Address;			//�ݧ��
	TempObjectItem.ID = RetObjData.ID;

	// Member List 
	// ObjectProperty�BClassProperty �����p
	if (RetObjData.Type.find("ObjectProperty") != std::string::npos or
		RetObjData.Type.find("ClassProperty") != std::string::npos
		) {
		ObjectMemberProc(RetObjData, TempMemberList, true, Preview, ProcessState, ProcessAddress);
		if (ProcessClass == ProcessClass::C_Inspector) return ProcessState::Retry;
	}
	// StructProperty �����p
	else if (RetObjData.Type.find("StructProperty") != std::string::npos
		) {
		ObjectMemberProc(RetObjData, TempMemberList, false, Preview, ProcessState, ProcessAddress);
		if (ProcessClass == ProcessClass::C_Inspector) return ProcessState::Retry;
	}
	// Array �����p
	else if (RetObjData.Type.find("ArrayProperty") != std::string::npos) {
		if (ProcessClass == ProcessClass::C_Inspector) {
			for (int i = 0; i < InspectorState.ObjectContentCreatEvent.Size; i++) {
				BasicDumperObject TempBasicObject;
				if (StorageMgr.GetObjectDict(RetObjData.Property[0].Address, TempObjData)) {
					InspectorAdditionalObjProc(TempObjData, TempBasicObject, i, i * CalcuTypeOffset(TempObjData.Type));
					TempMemberList.push_back(TempBasicObject);
				}
			}
		}
		else {
			ObjectMemberProc(RetObjData, TempMemberList, false);
		}
	}
	// Map ���p
	else if (RetObjData.Type.find("MapProperty") != std::string::npos) {
		if (ProcessClass == ProcessClass::C_Inspector) {
			size_t AccmulateOffset = 0x0;
			for (int i = 0; i < InspectorState.ObjectContentCreatEvent.Size; i++) {
				BasicDumperObject TempBasicObject;

				// Key
				if (StorageMgr.GetObjectDict(RetObjData.Property[0].Address, TempObjData)) {
					if (TempObjData.Type.find("EnumProperty") == std::string::npos and StorageMgr.GetObjectDict(TempObjData.Property[0].Address, TempObjData)) {} // �D�n�O Map �� Key �i��O��L Struct (Key �O�]���n�������X Type Name �ҥH�~�n�����AValue �h���ݭn)

					InspectorAdditionalObjProc(TempObjData, TempBasicObject, i, AccmulateOffset);
					AccmulateOffset += CalcuTypeOffset(TempBasicObject.Type);
					TempMemberList.push_back(TempBasicObject);
				}

				// Value
				if (StorageMgr.GetObjectDict(RetObjData.Property[1].Address, TempObjData)) {
					InspectorAdditionalObjProc(TempObjData, TempBasicObject, i, AccmulateOffset);
					AccmulateOffset += CalcuTypeOffset(TempBasicObject.Type);
					TempMemberList.push_back(TempBasicObject);
				}

				AccmulateOffset += ProcessInfo::ProcOffestAdd;
			}
		}
		else {
			DumperObjectWapper(TempObjData, TempBasicObject);
			TempMemberList.push_back(TempBasicObject);
		}
	}
	//�� Member �����p
	else {
		Address_Level_1 = RetObjData.MemberPtr.Address;
		while (true) {
			ObjectData MemberObj;
			if (StorageMgr.GetObjectDict(Address_Level_1, MemberObj, true)) {
				// ���ˬd (�p�G���ŦX�N���L�� Member)
				if (MemberObj.Type.find("Function") != std::string::npos or MemberObj.Offset < 0) {
					Address_Level_1 = MemMgr.MemReader.ReadMem<DWORD_PTR>(MemberObj.Address + UEOffset.NextMember); continue;
				}

				// Bool
				BasicDumperObject TempBasicObject;
				DumperObjectWapper(MemberObj, TempBasicObject);
				if (MemberObj.Type.find("BoolProperty") != std::string::npos)
					TempBasicObject.Bit = Utils.FindRightMostSetBit((int)MemberObj.BitMask);

				// Byte ���������ˬd
				if (MemberObj.Type.find("ByteProperty") != std::string::npos) {
					ObjectData PropertyObj;
					if (MemberObj.Property.size() >= 1 and StorageMgr.GetObjectDict(MemberObj.Property[0].Address, PropertyObj, true)) {
						ObjectEnumProc(PropertyObj, TempBasicObject, false);
					}
				}

				// SubType
				ObjectSubTypeProc(MemberObj.Address, TempBasicObject);

				// Enum
				if (Preview and MemberObj.Type.find("EnumProperty") != std::string::npos)
					ObjectEnumProc(MemberObj, TempBasicObject);

				// �� Inspector �ݭn�ˬd
				if (ProcessClass == ProcessClass::C_Inspector) {
					// Editor ���A���B�~��T
					if (InspectorState.ObjectContentCreatEvent.EditorEnable) {
						// Array �B Map
						if (MemberObj.Type.find("ArrayProperty") != std::string::npos or MemberObj.Type.find("MapProperty") != std::string::npos) {
							//Size
							int ObjectSize = MemMgr.MemReader.ReadMem<int>(InspectorState.ObjectContentCreatEvent.InstanceAddress + MemberObj.Offset + ProcessInfo::ProcOffestAdd);
							int AllocSize = MemMgr.MemReader.ReadMem<int>(InspectorState.ObjectContentCreatEvent.InstanceAddress + MemberObj.Offset + ProcessInfo::ProcOffestAdd + 4);

							TempBasicObject.Size.push_back(ObjectSize);
							TempBasicObject.Size.push_back(AllocSize);
						}
					}
				}
				TempMemberList.push_back(TempBasicObject);

				//�U�@�� Member
				Address_Level_1 = MemMgr.MemReader.ReadMem<DWORD_PTR>(MemberObj.Address + UEOffset.NextMember);
			}
			else break;
		}
	}
	TempObjectItem.Member = TempMemberList;
	return ProcessState::Completed;
}
