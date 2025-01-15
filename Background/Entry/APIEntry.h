#pragma once
#include <Windows.h>
#include "../Interface/ObjectInterface.h"
#include "../../GUI/Config/NotificationConfig.h"
#include "../../GUI/Config/MainConsoleConfig.h"
#include "../../State/EventHandler.h"
#include "../../State/GUIState.h"
#include "../Storage/StorageManager.h"
#include "../../System/Memory.h"
#include "../../Utils/Utils.h"
#include "../../Json/JsonManager.h"

namespace APIEntry
{
	void AddObject();
	void Update(DumperItem& APIItem, DWORD_PTR OriginAddress, DWORD_PTR NewAddress);
	void UpdateObjAddress();


	inline void Entry()
	{
		if (APIState.AddEvent.State == ProcessState::Start) {
			APIState.AddEvent.State = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Add API Object");
			AddObject();
			APIState.AddEvent.State = ProcessState::Completed;
		}
		if (APIState.UpdateEvent.State == ProcessState::Start) {
			APIState.UpdateEvent.State = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Update API BaseAddress");
			UpdateObjAddress();
			APIState.UpdateEvent.State = ProcessState::Completed;
		}
	}

	inline void FileEntry()
	{
		if (APIState.OpenFileEvent.State == ProcessState::Start) {
			APIState.OpenFileEvent.State = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Open API File");
			JsonMgr.ReadAPIFile();
			APIState.OpenFileEvent.State = ProcessState::Completed;
		}
		if (APIState.SaveFileEvent.State == ProcessState::Start) {
			APIState.SaveFileEvent.State = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Save API File");
			JsonMgr.WriteAPIFile();
			APIState.SaveFileEvent.State = ProcessState::Completed;
		}
	}	
}

void APIEntry::AddObject()
{
	int ExistFlag = false;
	ObjectData InstanceObjectData;
	StorageMgr.GetObjectDict(APIState.AddEvent.BaseAddress, InstanceObjectData, true);

	// �P�_�O�_�s�b�� APIItemList
	DWORD_PTR InstanceAddress = InstanceObjectData.Address;
	DumperItem* Object = NULL;
	BasicDumperObject BasicObject;
	ExistFlag = -1;
	for (int i = 0; i < APIConf.APIItemList.size(); i++) {
		if (APIConf.APIItemList[i].Address == InstanceObjectData.Address) {
			ExistFlag = i;
			break;
		}
	}

	if (ExistFlag != -1) {
		Object = &APIConf.APIItemList[ExistFlag];
	}
	else {
		// creat new
		DumperItem APIListObject;
		APIListObject.Name = InstanceObjectData.Name;
		APIListObject.Type = InstanceObjectData.Type;	//�򥻤W Type �N�O�|�O ReferenceObjName�A�|�O�ۦP�W��
		APIListObject.FullName = InstanceObjectData.FullName;
		APIListObject.Type = InstanceObjectData.Type;
		APIListObject.ID = InstanceObjectData.ID;
		APIListObject.Address = InstanceObjectData.Address;
		APIConf.APIItemList.push_back(APIListObject);

		// basic info
		Object = &APIConf.APIItemList[APIConf.APIItemList.size() - 1];
		Object->ObjectMap[InstanceAddress].Name = InstanceObjectData.Name;
		Object->ObjectMap[InstanceAddress].Address = InstanceObjectData.Address;
		Object->ObjectMap[InstanceAddress].Offset = InstanceObjectData.Offset;
	}

	// Is Pointer �N�s�W DepthPath ���|�W�Ҧ����`�I
	DWORD_PTR TempAddress = InstanceObjectData.Address;
	std::vector<std::pair<DWORD_PTR, size_t>>& DepthPath = APIState.AddEvent.DepthPath;
	DepthPath.push_back(std::make_pair(APIState.AddEvent.Address + APIState.AddEvent.Offset, APIState.AddEvent.ObjectAddress));		// �̫�@�ӴN�O�ؼЭn�x�s���Ѽ�
	for (int i = 0; i < DepthPath.size(); i++) {
		ObjectData TempRootObjectData, TempObjectData;
		StorageMgr.GetObjectDict(TempAddress, TempRootObjectData, true);
		StorageMgr.GetObjectDict(DepthPath[i].second, TempObjectData, true);


		// If the ID does not exist in the map, initialize the object in the map
		if (Object->ObjectMap.find(InstanceAddress) == Object->ObjectMap.end()) {
			Object->ObjectMap[InstanceAddress].Name = TempRootObjectData.Name;
			Object->ObjectMap[InstanceAddress].Address = InstanceAddress;
			Object->ObjectMap[InstanceAddress].Offset = TempRootObjectData.Offset;
		}

		// if already exist
		ExistFlag = false;
		for (int j = 0; j < Object->ObjectMap[InstanceAddress].Member.size(); j++) {
			if (Object->ObjectMap[InstanceAddress].Member[j].ObjectAddress == TempObjectData.Address) {
				// �̫�@�Ӫ�ܰѼơA��~�ˬd bitmask �O�_�ۦP�ӧP�_�O�_�O�ؼаѼ�
				if (i == (DepthPath.size() - 1) and Object->ObjectMap[InstanceAddress].Member[j].Bit != Utils.FindRightMostSetBit((int)TempObjectData.BitMask)) break;
				ExistFlag = true;
				break;
			}
		}

		if (!ExistFlag) {	// add member if not exist
			BasicObject.Name = TempObjectData.Name;
			BasicObject.Type = TempObjectData.Type;
			BasicObject.ID = TempObjectData.ID;
			BasicObject.Offset = TempObjectData.Offset;
			BasicObject.Address = DepthPath[i].first;
			BasicObject.ObjectAddress = TempObjectData.Address;
			BasicObject.Bit = Utils.FindRightMostSetBit((int)TempObjectData.BitMask);
			Object->ObjectMap[InstanceAddress].Member.push_back(BasicObject);
		}

		// Update ID (�����i�h�U�@�h Object ���ʧ@)
		TempAddress = TempObjectData.Address;
		InstanceAddress = DepthPath[i].first;
	}
	DepthPath.pop_back();
}

void APIEntry::Update(DumperItem& APIItem, DWORD_PTR OriginAddress, DWORD_PTR NewAddress) {
	auto it = APIItem.ObjectMap.find(OriginAddress);
	if (it != APIItem.ObjectMap.end()) {
		APIItem.ObjectMap[NewAddress] = it->second;	//�s�W
		APIItem.ObjectMap.erase(it);	//�R���ª�

		APIItem.ObjectMap[NewAddress].Address = NewAddress;
		for (BasicDumperObject& memberObj : APIItem.ObjectMap[NewAddress].Member) {
			OriginAddress = memberObj.Address;
			MemMgr.MemReader.ReadMem(memberObj.Address, NewAddress + memberObj.Offset);
			Update(APIItem, OriginAddress, memberObj.Address);
		}
	}
}

void APIEntry::UpdateObjAddress()
{
	// init
	InspectorConf.ObjectInstanceSearchMap.clear();

	for (int i = 0; i < APIConf.APIItemList.size(); i++) {
		DumperItem& APIItem = APIConf.APIItemList[i];
		std::string ReferenceObjectName = APIItem.Type;
		ObjectData ObjData;
		DWORD_PTR TargetAddress = NULL;
		DWORD_PTR ObjectAddress = NULL;

		// �ΦW�l�h���M����
		if (StorageMgr.GetObjectDictByName(ReferenceObjectName, ObjData)) {
			// �p�G Name�BType �ۦP�A�h�� dynamic Object�A�� Address �Y������
			if (ObjData.Type == ObjData.Name)
				TargetAddress = ObjData.Address;

			// TODO
			// �p�G���P�h�ھ� Type �M�w���᪺�ѪR���k
			else {
				if (ObjData.Type.find("ObjectProperty") != std::string::npos) {
					ObjectAddress = ObjData.Property[0].Address;

					// Address Scan
					BYTE BytesArray[sizeof(DWORD_PTR)];
					MemMgr.AddressToBytes(ObjectAddress, BytesArray);
					std::vector<DWORD_PTR> ScanResult;
					MemMgr.Scanner.AOBSCAN(BytesArray, sizeof(DWORD_PTR), ScanResult, true, true);

					// Filter
					DWORD_PTR InstanceAddress = NULL;
					ObjectData TempObjectData;
					std::vector<DWORD_PTR> FilterResult;
					for (int i = 0; i < ScanResult.size(); i++) {
						InstanceAddress = ScanResult[i] - 0x10;
						//�O����
						if (StorageMgr.GetObjectDict(InstanceAddress, TempObjectData, true)) {
							// �O�ؼЪ��� (�W�l�B������|�BInstance�ѦҪ�����W�ٳ��ۦP)
							if (APIItem.Name == TempObjectData.Name and
								APIItem.Type == TempObjectData.Type and
								APIItem.FullName == TempObjectData.FullName)
								FilterResult.push_back(InstanceAddress);
						}
					}
					if (FilterResult.size() > 0)
						TargetAddress = FilterResult[0];
				}
			}
		}

		// Update
		std::stack<DWORD_PTR> AddressStack;
		DWORD_PTR OriginAddress = APIItem.Address;
		APIItem.Address = TargetAddress;

		Update(APIItem, OriginAddress, TargetAddress);
	}
}