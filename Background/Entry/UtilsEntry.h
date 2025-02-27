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
	inline bool IsObjectNameInPackage(std::string TargetObjectName, DWORD_PTR* Address);

	inline size_t CalcuTypeOffset(std::string Type);


	inline DWORD_PTR PropertySelector(ObjectData& Object);


	inline bool ObjectInfoWapper(BasicDumperInfoObject& ObjectA, BasicObjectData& ObjectB);


	inline void InspectorAdditionalObjProc(ObjectData& Object, BasicDumperObject& TempBasicObject, int LoopIndex, size_t Offset);


	inline bool ObjectSubTypeProc(DWORD_PTR Address, BasicDumperObject& TempBasicObject, bool CheckClickable = false);


	inline bool GetEnumList(DWORD_PTR Address, std::vector<std::pair<std::string, size_t>>& EnumList);


	inline void ObjectEnumProc(ObjectData& EnumObject, BasicDumperObject& TempBasicObject, bool IsPropertyObject = true);


	inline void ObjectMemberProcessSelector(DWORD_PTR Address, std::vector<BasicDumperObject>& MemberList, bool Preview = false, ProcessState* ProcessState = NULL, DWORD_PTR* ProcessAddress = NULL);


	inline bool ObjectMemberProc(ObjectData& RetObjData, std::vector<BasicDumperObject>& MemberList, bool IsObject, bool Preview = false, ProcessState* ProcessState = NULL, DWORD_PTR* ProcessAddress = NULL);


	inline ProcessState ObjectMemberListProc(ProcessClass ProcessClass, ObjectData& RetObjData, DumperObject& TempObjectItem, bool Preview = false, ProcessState* ProcessState = NULL, DWORD_PTR* ProcessAddress = NULL);

}


bool UtilsEntry::IsObjectNameInPackage(std::string TargetObjectName, DWORD_PTR* Address = nullptr) {
	bool found = false;

	// 在多層 map 中尋找特定的 ObjectName
	for (auto& NamesEntry : StorageMgr.GetPackage()) {
		for (auto& TypeEntry : NamesEntry.second) {
			for (auto& ObjectEntry : TypeEntry.second) {
				std::string ObjectName = ObjectEntry.second.first;
				if (ObjectName == TargetObjectName) {
					found = true;
					if(Address) *Address = ObjectEntry.second.second;
					break; // 若找到目標 ObjectName，則結束迴圈
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
		Type.find("BoolProperty") != std::string::npos		//<= bool 有待檢查
		)
	{
		return 0x1;
	}

	return 0x0; //目前還不支援的物件型態直接略過
}

DWORD_PTR UtilsEntry::PropertySelector(ObjectData& Object) {
	return (Object.Property.size() >= 2) ? Object.Property[1].Address : Object.Property[0].Address;
}


bool UtilsEntry::ObjectInfoWapper(BasicDumperInfoObject& ObjectA, BasicObjectData& ObjectB)
{
	ObjectA.Name = ObjectB.Name;
	ObjectA.Type = ObjectB.Type;
	ObjectA.Offset = ObjectB.Offset;
	ObjectA.Address = ObjectB.Address;
	ObjectA.Value = ObjectB.Address;
	ObjectA.InstanceAddress = ObjectB.Address;
	ObjectA.ID = ObjectB.ID;
	return true;
}

void UtilsEntry::InspectorAdditionalObjProc(ObjectData& Object, BasicDumperObject& TempBasicObject, int LoopIndex, size_t Offset)
{
	// Warpper Basic Infomation
	ObjectInfoWapper(TempBasicObject, Object);

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
	// 主要是 Array 底下可能是其他 Struct
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
			TempSubTypeObject.Clickable = (CheckClickable ? IsObjectNameInPackage(TempObjData.Name) : false);
			TempBasicObject.SubType.push_back(TempSubTypeObject);
		}
	}
	return true;
}

bool UtilsEntry::GetEnumList(DWORD_PTR Address, std::vector<std::pair<std::string, size_t>>& EnumList)
{
	DWORD_PTR Address_Level_1;
	std::string EnumName;
	size_t EnumIndex, EnumFNameIndex;		// EnumIndex 好像不會用到，畢竟 Emun 大多都是從 0 開始
	DWORD_PTR EnumListAddress = NULL;
	MemMgr.MemReader.ReadMem(EnumListAddress, Address + UEOffset.EnumList);
	int EnumListSize = 0;
	MemMgr.MemReader.ReadMem(EnumListSize, Address + UEOffset.EnumList + ProcessInfo::ProcOffestAdd);
	for (int i = 0; i < EnumListSize; i++) {
		Address_Level_1 = EnumListAddress + i * UEOffset.EnumPropMul;
		MemMgr.MemReader.ReadMem(EnumFNameIndex, Address_Level_1 + UEOffset.EnumPropName);
		FNameParser.GetFNameStringByID(EnumFNameIndex, EnumName, true);
		MemMgr.MemReader.ReadMem(EnumIndex, Address_Level_1 + UEOffset.EnumPropIndex);
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
		ObjectInfoWapper(BasicObject, ObjData);
		MemberList.push_back(BasicObject);
	}
	else {
		// Preview => 設定相關參數後返回，跳下一回合
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

	// 否則就取 TypeObject
	else {
		MemMgr.MemReader.ReadMem(Address_Level_1, RetObjData.Address + UEOffset.TypeObject);
		StorageMgr.GetObjectDict(Address_Level_1, TempObjData);

		// 如果有 Member 則直接回傳該物件  || 如果是已經是 Object 
		if (IsObject or TempObjData.MemberPtr.Address != NULL)
			ObjectMemberProcessSelector(TempObjData.Address, MemberList, Preview, ProcessState, ProcessAddress);
		// 如果有 Property
		else if (TempObjData.Property.empty())
			ObjectMemberProcessSelector(TempObjData.Property[0].Address, MemberList, Preview, ProcessState, ProcessAddress);
		// 如果沒有 Property => 看 super
		else if (StorageMgr.GetObjectDict(TempObjData.SuperPtr.Address, TempObjData))	// 先驗證 SuperPtr.Address 有效
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
	std::vector<BasicDumperObject> TempMemberList;

	// Basic Data
	ObjectInfoWapper(TempObjectItem, RetObjData);

	// Member List 
	// ObjectProperty、ClassProperty 的狀況
	if (RetObjData.Type.find("ObjectProperty") != std::string::npos or
		RetObjData.Type.find("ClassProperty") != std::string::npos
		) {
		ObjectMemberProc(RetObjData, TempMemberList, true, Preview, ProcessState, ProcessAddress);
		if (ProcessClass == ProcessClass::C_Inspector) return ProcessState::Retry;
	}
	// StructProperty 的狀況
	else if (RetObjData.Type.find("StructProperty") != std::string::npos
		) {
		ObjectMemberProc(RetObjData, TempMemberList, false, Preview, ProcessState, ProcessAddress);
		if (ProcessClass == ProcessClass::C_Inspector) return ProcessState::Retry;
	}
	// Array 的狀況
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
	// Map 狀況
	else if (RetObjData.Type.find("MapProperty") != std::string::npos) {
		BasicDumperObject TempBasicObject;
		if (ProcessClass == ProcessClass::C_Inspector) {
			size_t AccmulateOffset = 0x0;
			for (int i = 0; i < InspectorState.ObjectContentCreatEvent.Size; i++) {
				// Key
				if (StorageMgr.GetObjectDict(RetObjData.Property[0].Address, TempObjData)) {
					if (TempObjData.Type.find("EnumProperty") == std::string::npos and StorageMgr.GetObjectDict(TempObjData.Property[0].Address, TempObjData)) {} // 主要是 Map 的 Key 可能是其他 Struct (Key 是因為要直接取出 Type Name 所以才要先做，Value 則不需要)

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
			ObjectInfoWapper(TempBasicObject, TempObjData);
			TempMemberList.push_back(TempBasicObject);
		}
	}
	//有 Member 的狀況
	else {
		Address_Level_1 = RetObjData.MemberPtr.Address;
		while (true) {
			ObjectData MemberObj;
			if (StorageMgr.GetObjectDict(Address_Level_1, MemberObj, true)) {
				// 基本檢查 (如果不符合就跳過該 Member)
				if (MemberObj.Type.find("Function") != std::string::npos or MemberObj.Offset < 0) {
					MemMgr.MemReader.ReadMem(Address_Level_1, MemberObj.Address + UEOffset.NextMember); continue;
				}

				// Bool
				BasicDumperObject TempBasicObject;
				ObjectInfoWapper(TempBasicObject, MemberObj);
				if (MemberObj.Type.find("BoolProperty") != std::string::npos)
					TempBasicObject.Bit = Utils.FindRightMostSetBit((int)MemberObj.BitMask);

				// Byte 延伸物件檢查
				if (MemberObj.Type.find("ByteProperty") != std::string::npos) {
					ObjectData PropertyObj;
					if (MemberObj.Property.size() >= 1 and StorageMgr.GetObjectDict(MemberObj.Property[0].Address, PropertyObj, true)) {
						ObjectEnumProc(PropertyObj, TempBasicObject, false);
					}
				}

				// SubType
				ObjectSubTypeProc(MemberObj.Address, TempBasicObject, true);

				// Member Clickable
				if (TempBasicObject.Type.find("Property") == std::string::npos and UtilsEntry::IsObjectNameInPackage(TempBasicObject.Type)) 
					TempBasicObject.Clickable = true;

				// Enum
				if (Preview and MemberObj.Type.find("EnumProperty") != std::string::npos)
					ObjectEnumProc(MemberObj, TempBasicObject);

				// 僅 Inspector 需要檢查
				if (ProcessClass == ProcessClass::C_Inspector) {
					// Editor 狀態的額外資訊
					if (InspectorState.ObjectContentCreatEvent.EditorEnable) {
						// Array 、 Map
						if (MemberObj.Type.find("ArrayProperty") != std::string::npos or MemberObj.Type.find("MapProperty") != std::string::npos) {
							//Size
							int ObjectSize = 0, AllocSize = 0;
							MemMgr.MemReader.ReadMem(ObjectSize, InspectorState.ObjectContentCreatEvent.InstanceAddress + MemberObj.Offset + ProcessInfo::ProcOffestAdd);
							MemMgr.MemReader.ReadMem(AllocSize, InspectorState.ObjectContentCreatEvent.InstanceAddress + MemberObj.Offset + ProcessInfo::ProcOffestAdd + 4);

							TempBasicObject.Size.push_back(ObjectSize);
							TempBasicObject.Size.push_back(AllocSize);
						}
					}
				}
				TempMemberList.push_back(TempBasicObject);

				//下一個 Member
				MemMgr.MemReader.ReadMem(Address_Level_1, MemberObj.Address + UEOffset.NextMember);
			}
			else break;
		}
	}
	TempObjectItem.Member = TempMemberList;
	return ProcessState::Completed;
}
