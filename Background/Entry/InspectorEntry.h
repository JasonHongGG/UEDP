#pragma once
#include <shared_mutex>
#include "../../GUI/Config/NotificationConfig.h"
#include "../../GUI/Config/MainConsoleConfig.h"
#include "../../State/EventHandler.h"
#include "../../State/GUIState.h"
#include "../Core/Parser/FNamePoolParser.h"
#include "../../System/Memory.h"
#include "UtilsEntry.h"

namespace InspectorEntry
{
	inline std::shared_mutex  GetObjectInstanceSearchLock;

	inline void GetFName();
	inline void AddObjectInspector();
	inline void CreatObectNamePage();
	inline bool ConditionValidation(std::vector < InspectorConfig::ConditionObject >& ConditionSet);
	inline bool ScanFilter(std::vector < InspectorConfig::ConditionObject >& ConditionSet, DWORD_PTR Address);
	inline void ObjectInstanceSearch();

	inline void Entry()
	{
		if (InspectorState.GetFNameEvent.State == ProcessState::Start) {
			InspectorState.GetFNameEvent.State = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Editor Get FName");
			GetFName();
			InspectorState.GetFNameEvent.State = ProcessState::Completed;
		}

		if (InspectorState.AddInspectorEvent.State == ProcessState::Start) { //�u���� Basic Data
			InspectorState.AddInspectorEvent.State = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Add Object Inspector");
			AddObjectInspector();
			InspectorState.AddInspectorEvent.State = ProcessState::Completed;
		}

		if (InspectorState.ObjectContentCreatEvent.State == ProcessState::Start or InspectorState.ObjectContentCreatEvent.State == ProcessState::Retry) {
			InspectorState.ObjectContentCreatEvent.State = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Creat ObectName Page");
			CreatObectNamePage();
			if (InspectorState.ObjectContentCreatEvent.State == ProcessState::Retry) return;
			InspectorState.ObjectContentCreatEvent.State = ProcessState::Completed;
			// �B�z����q�� Menu �i�H���
			InspectorState.ObjectContentOpenEvent.State = ProcessState::Completed;
		}

		if (InspectorState.ObjectInstanceSearchEvent.State == ProcessState::Start) {
			InspectorState.ObjectInstanceSearchEvent.State = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Object Instance Search");
			ObjectInstanceSearch();
			InspectorState.ObjectInstanceSearchEvent.State = ProcessState::Completed;
		}
	}
}


void InspectorEntry::GetFName()
{
	//�P�_��J�椣����
	if (InspectorState.GetFNameEvent.FNameID != 0) {
		std::string FName;
		if (FNameParser.GetFNameStringByID<long long int>(InspectorState.GetFNameEvent.FNameID, FName, true)) {
			EditorMemMgr.ObjectStroageData[InspectorState.GetFNameEvent.Address].NameType[InspectorState.GetFNameEvent.FNameID] = FName;
		}
		else EditorMemMgr.ObjectStroageData[InspectorState.GetFNameEvent.Address].NameType[InspectorState.GetFNameEvent.FNameID] = "None";
	}
}


void InspectorEntry::AddObjectInspector()
{
	// �z�L Address ���ը��X Object
	ObjectData RetObjData;
	if (StorageMgr.GetObjectDict(InspectorState.AddInspectorEvent.Address, RetObjData, true))
	{
		// �p�G�w�s�b�N���� Return
		for (int i = 0; i < InspectorConf.InspectorList.size(); i++) {
			if (InspectorConf.InspectorList[i].Name == RetObjData.Name) return;
		}

		// �p�G���b ObjectInspectorList�A�h�x�s
		// �p�G�O Derived Mode ���B�~���X�Ҧ��� Derived Class ��J Derived List
		InspectorConf.InspectorList.push_back({ RetObjData.Name, RetObjData.Type, InspectorState.AddInspectorEvent.Address, InspectorConf.InspectorListMode, InspectorState.AddInspectorEvent.EditorEnable, InspectorState.AddInspectorEvent.IsInstance });
		if (InspectorConf.InspectorListMode == InspectorConfig::InspectorListMode::Derived) {
			InspectorConf.DerivedList.Map.clear();
			StorageMgr.SetObjectUper(RetObjData.Address);					//��s Upper
			StorageMgr.GetObjectDict(RetObjData.Address, RetObjData);		//��s����A���s���@������
			for (int i = 0; i < RetObjData.Uper.size(); i++) {
				InspectorConf.DerivedList.Map[RetObjData.Name].push_back({
					RetObjData.Uper[i].Name,
					RetObjData.Uper[i].Type,
					RetObjData.Uper[i].Address,
					InspectorConf.InspectorListMode,
					InspectorState.AddInspectorEvent.EditorEnable ,
					InspectorState.AddInspectorEvent.IsInstance
					});
			}
		}
	}
	else {
		EventHandler::NotifyEvent(NotificationConfig::NotiyType::Error, "Invalid address", "Address does not map to any existing object");
	}
}


void InspectorEntry::CreatObectNamePage()
{
	// �ˬd�O�_�w�s�b
	if (InspectorConf.InspectorObjectContentMap.find(InspectorState.ObjectContentCreatEvent.Name) != InspectorConf.InspectorObjectContentMap.end()) {
		return;
	}

	ObjectData RetObjData;
	ObjectData TempObjData;
	DWORD_PTR Address_Level_1 = NULL;

	// ����e���ˬd�O Instance �٬O Object
	DWORD_PTR EntryAddress = InspectorState.ObjectContentCreatEvent.Address;
	if (StorageMgr.GetObjectDict(EntryAddress, TempObjData, true)) { // ���\���o����
		MessageObject& MsgObj = InspectorState.ObjectContentCreatEvent;
		if (MsgObj.InstanceAddress != 0x0 and // ���O�Ū�
			MsgObj.InstanceAddress != MsgObj.Address and // // ���O openEvent
			(MsgObj.Type.find("ObjectProperty") != std::string::npos or
				MsgObj.Type.find("ClassProperty") != std::string::npos)
			)
			MemMgr.MemReader.ReadMem(EntryAddress, MsgObj.InstanceAddress + 0x10); //���f�I���� 0x10

		//else if (
		//	(TempObjData.FullName.find("/Game/") != std::string::npos or TempObjData.FullName.find("/Engine/") != std::string::npos) and							//�O Game ������
		//	TempObjData.ClassPtr.Name.compare("BlueprintGeneratedClass") != 0 and						// Type ���� Class �� Root Object
		//	TempObjData.ClassPtr.Name.compare("Class") != 0)
		//	MemMgr.MemReader.ReadMem(EntryAddress, EntryAddress + 0x10); //���f�I���� 0x10

		else if (InspectorState.ObjectContentCreatEvent.IsInstance)
			MemMgr.MemReader.ReadMem(EntryAddress, EntryAddress + 0x10);
	}

	// Main
	if (StorageMgr.GetObjectDict(EntryAddress, RetObjData, true)) {
		Address_Level_1 = RetObjData.Address;
		while (true) {
			DumperObject TempObjectItem;
			StorageMgr.GetObjectDict(Address_Level_1, RetObjData, true);
			if (UtilsEntry::ObjectMemberListProc(ProcessClass::C_Inspector, RetObjData, TempObjectItem, true, &InspectorState.ObjectContentCreatEvent.State, &InspectorState.ObjectContentCreatEvent.Address) == ProcessState::Retry) return;

			//�x�s���G "�~��Class Chain" ���䤤�@�� Node
			InspectorConf.InspectorObjectContentMap[InspectorState.ObjectContentCreatEvent.Name].push_back(TempObjectItem);

			//�U�@�h Super	(�p�G�O Class �~��Ҧ� Super Class)
			if (Utils.rStringToLower(RetObjData.Type).find("class") != std::string::npos or Utils.rStringToLower(RetObjData.Type).find("struct") != std::string::npos) {	// �O class �� struct ���n���j�Ҧ��~�Ӫ���
				Address_Level_1 = RetObjData.SuperPtr.Address;
				if (Address_Level_1 == NULL) break;
			}
			else break;
		}
	}
}


bool InspectorEntry::ConditionValidation(std::vector < InspectorConfig::ConditionObject >& ConditionSet) {

	bool failureFlag = false;

	for (int j = 0; j < ConditionSet.size(); j++) {
		bool PointerEnable = ConditionSet[j].PointerEnable;
		std::string OffsetStr(ConditionSet[j].Offset);
		std::string ValueStr(ConditionSet[j].Value);

		if (ValueStr.empty()) EventHandler::NotifyEvent(NotificationConfig::NotiyType::Warning, Utils.StringWrapper("[ Skip Condition %d ]", j), "Value Is Empty");
		if (OffsetStr.empty()) EventHandler::NotifyEvent(NotificationConfig::NotiyType::Warning, Utils.StringWrapper("[Skip Condition % d]", j), "Base Offset Is Empty");
		else if (!Utils.IsHex(OffsetStr) || !Utils.IsHex(OffsetStr)) EventHandler::NotifyEvent(NotificationConfig::NotiyType::Warning, Utils.StringWrapper("[Skip Condition % d]", j), "Base Offset Is Not A Number");
		if (ValueStr.empty() || OffsetStr.empty() || !Utils.IsHex(OffsetStr) || !Utils.IsHex(OffsetStr)) failureFlag = true;


		if (PointerEnable) {
			std::string PointerStr;
			for (int k = 0; k < ConditionSet[j].Pointer.size(); k++) {	// Pointer // n ���j��
				PointerStr = ConditionSet[j].Pointer[k];
				if (PointerStr.empty()) EventHandler::NotifyEvent(NotificationConfig::NotiyType::Warning, Utils.StringWrapper("[Skip Condition % d]", j), Utils.StringWrapper("Pointer %d Is Empty", k));
				else if (!Utils.IsHex(PointerStr) || !Utils.IsHex(PointerStr))  EventHandler::NotifyEvent(NotificationConfig::NotiyType::Warning, Utils.StringWrapper("[Skip Condition % d]", j), Utils.StringWrapper("Pointer %d Is Not A Number", k));
				if (PointerStr.empty() || !Utils.IsHex(PointerStr) || !Utils.IsHex(PointerStr)) failureFlag = true;

			}
		}
	}

	return failureFlag;

}

bool InspectorEntry::ScanFilter(std::vector < InspectorConfig::ConditionObject >& ConditionSet, DWORD_PTR Address) {

	bool IgnoreFlag = false;

	for (int j = 0; j < ConditionSet.size(); j++) {		// Condition
		ValueType Type = ConditionSet[j].Type;
		bool PointerEnable = ConditionSet[j].PointerEnable;
		std::string OffsetStr(ConditionSet[j].Offset);
		std::string ValueStr(ConditionSet[j].Value);
		if (ValueStr.empty() || OffsetStr.empty() || !Utils.IsHex(OffsetStr) || !Utils.IsHex(OffsetStr)) continue;

		size_t BaseOffset = Utils.StringToHex(OffsetStr);
		size_t LastOffset = BaseOffset;

		if (PointerEnable) {
			size_t Pointer;
			std::string PointerStr;
			if (!MemMgr.MemReader.ReadMem(Address, Address + BaseOffset))
			{
				EventHandler::NotifyEvent(NotificationConfig::NotiyType::Warning, Utils.StringWrapper("[Skip Condition % d]", j), "[Base Offset] Invalid Memory Address"); 
				continue; 
			}

			int k = 0;
			for (k = 0; k < ConditionSet[j].Pointer.size(); k++) {	// Pointer // n ���j��
				PointerStr = ConditionSet[j].Pointer[k];
				if (PointerStr.empty() || !Utils.IsHex(PointerStr) || !Utils.IsHex(PointerStr)) break;
				Pointer = Utils.StringToHex(PointerStr);

				// �O�_�O�̫�@��
				if (k == ConditionSet[j].Pointer.size() - 1) { LastOffset = Pointer; break; }

				// �U�@��
				if (!MemMgr.MemReader.ReadMem(Address, Address + Pointer))
				{ 
					EventHandler::NotifyEvent(NotificationConfig::NotiyType::Warning, Utils.StringWrapper("[Skip Condition % d]", j), Utils.StringWrapper("[ Pointer %d ] Invalid memory address", k)); 
					break; 
				}
			}
			if (k != ConditionSet[j].Pointer.size() - 1) continue;	// �j�馳�S����� n-1 ��
		}

		// ���O�� 16 �i����
		if (Type == ValueType::Byte) {
			BYTE Value = std::stoi(ValueStr, nullptr, 10);
			if (!MemMgr.MemReader.IsEqual<BYTE>(Address + LastOffset, Value)) { IgnoreFlag = true; break; };
		}
		if (Type == ValueType::Int16) {
			int16_t Value = std::stoi(ValueStr, nullptr, 10);
			if (!MemMgr.MemReader.IsEqual<int16_t>(Address + LastOffset, Value)) { IgnoreFlag = true; break; };
		}
		else if (Type == ValueType::Int32) {
			int Value = std::stoi(ValueStr, nullptr, 10);
			if (!MemMgr.MemReader.IsEqual<int>(Address + LastOffset, Value)) { IgnoreFlag = true; break; };
		}
		else if (Type == ValueType::Int64) {
			int64_t Value = std::stoi(ValueStr, nullptr, 10);
			if (!MemMgr.MemReader.IsEqual<int64_t>(Address + LastOffset, Value)) { IgnoreFlag = true; break; };
		}
		else if (Type == ValueType::Float) {
			float Value = std::stof(ValueStr);
			if (!MemMgr.MemReader.IsEqual<float>(Address + LastOffset, Value)) { IgnoreFlag = true; break; };
		}
		else if (Type == ValueType::Double) {
			double Value = std::stod(ValueStr);
			if (!MemMgr.MemReader.IsEqual<double>(Address + LastOffset, Value)) { IgnoreFlag = true; break; };
		}
	}

	if (IgnoreFlag) return false;
	else return true;
}

void InspectorEntry::ObjectInstanceSearch()
{
	if (InspectorState.ObjectInstanceSearchEvent.SearchStr.length() != 0) {
		DWORD_PTR Address = std::stoull(InspectorState.ObjectInstanceSearchEvent.SearchStr, nullptr, 16);

		// Condition Validation 
		if (InspectorConf.ObjectInstanceSearchMode == InspectorConfig::SearchMode::Condition)
			if (ConditionValidation(InspectorConf.ConditionSet)) return;		// ���ҥ��ѴN�����d��

		// Address Scan
		BYTE BytesArray[sizeof(DWORD_PTR)];
		MemMgr.AddressToBytes(Address, BytesArray);
		std::vector<DWORD_PTR> ScanResult;
		MemMgr.Scanner.AOBSCAN(BytesArray, sizeof(DWORD_PTR), ScanResult, true, true);

		// Filter and SaveObj
		DWORD_PTR InstanceAddress = 0x0;
		DWORD_PTR ResultAddress = 0x0;
		GetObjectInstanceSearchLock.lock();
		InspectorConf.ObjectInstanceSearchMap.clear();
		for (int i = 0; i < ScanResult.size(); i++) {
			InstanceAddress = ScanResult[i] - 0x10;	// Object Address - 0x10 = Instance Address
			ResultAddress = InstanceAddress;

			// �ˬd�O�_�� Object Reference �O�_�s�b
			ObjectData TempObjData;
			if (!StorageMgr.GetObjectDict(ResultAddress, TempObjData, true)) continue;
			if (TempObjData.Name == "InvalidName" or TempObjData.Name == "None") continue;

			// �ˬd Condition �O�_�q�L
			if (InspectorConf.ObjectInstanceSearchMode == InspectorConfig::SearchMode::Condition)
				if (!ScanFilter(InspectorConf.ConditionSet, InstanceAddress)) continue; //����@�ӱ���S�L�N������󵲪G

			InspectorConf.ObjectInstanceSearchMap[Utils.HexToString(ResultAddress)] = ResultAddress;
		}
		GetObjectInstanceSearchLock.unlock();
	}
}
