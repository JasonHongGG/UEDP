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

		if (InspectorState.AddInspectorEvent.State == ProcessState::Start) { //只先拿 Basic Data
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
			// 處理完後通知 Menu 可以顯示
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
	//判斷輸入欄不為空
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
	// 透過 Address 嘗試取出 Object
	ObjectData RetObjData;
	if (StorageMgr.GetObjectDict(InspectorState.AddInspectorEvent.Address, RetObjData, true))
	{
		// 如果已存在就直接 Return
		for (int i = 0; i < InspectorConf.InspectorList.size(); i++) {
			if (InspectorConf.InspectorList[i].Name == RetObjData.Name) return;
		}

		// 如果不在 ObjectInspectorList，則儲存
		// 如果是 Derived Mode 需額外取出所有的 Derived Class 放入 Derived List
		InspectorConf.InspectorList.push_back({ RetObjData.Name, RetObjData.Type, InspectorState.AddInspectorEvent.Address, InspectorConf.InspectorListMode, InspectorState.AddInspectorEvent.EditorEnable, InspectorState.AddInspectorEvent.IsInstance });
		if (InspectorConf.InspectorListMode == InspectorConfig::InspectorListMode::Derived) {
			InspectorConf.DerivedList.Map.clear();
			StorageMgr.SetObjectUper(RetObjData.Address);					//更新 Upper
			StorageMgr.GetObjectDict(RetObjData.Address, RetObjData);		//更新完後再重新取一次物件
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
	// 檢查是否已存在
	if (InspectorConf.InspectorObjectContentMap.find(InspectorState.ObjectContentCreatEvent.Name) != InspectorConf.InspectorObjectContentMap.end()) {
		return;
	}

	ObjectData RetObjData;
	ObjectData TempObjData;
	DWORD_PTR Address_Level_1 = NULL;

	// 執行前先檢查是 Instance 還是 Object
	DWORD_PTR EntryAddress = InspectorState.ObjectContentCreatEvent.Address;
	if (StorageMgr.GetObjectDict(EntryAddress, TempObjData, true)) { // 成功取得物件
		MessageObject& MsgObj = InspectorState.ObjectContentCreatEvent;
		if (MsgObj.InstanceAddress != 0x0 and // 不是空的
			MsgObj.InstanceAddress != MsgObj.Address and // // 不是 openEvent
			(MsgObj.Type.find("ObjectProperty") != std::string::npos or
				MsgObj.Type.find("ClassProperty") != std::string::npos)
			)
			MemMgr.MemReader.ReadMem(EntryAddress, MsgObj.InstanceAddress + 0x10); //路口點偏移 0x10

		//else if (
		//	(TempObjData.FullName.find("/Game/") != std::string::npos or TempObjData.FullName.find("/Engine/") != std::string::npos) and							//是 Game 的物件
		//	TempObjData.ClassPtr.Name.compare("BlueprintGeneratedClass") != 0 and						// Type 不為 Class 的 Root Object
		//	TempObjData.ClassPtr.Name.compare("Class") != 0)
		//	MemMgr.MemReader.ReadMem(EntryAddress, EntryAddress + 0x10); //路口點偏移 0x10

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

			//儲存結果 "繼承Class Chain" 的其中一個 Node
			InspectorConf.InspectorObjectContentMap[InspectorState.ObjectContentCreatEvent.Name].push_back(TempObjectItem);

			//下一層 Super	(如果是 Class 才找所有 Super Class)
			if (Utils.rStringToLower(RetObjData.Type).find("class") != std::string::npos or Utils.rStringToLower(RetObjData.Type).find("struct") != std::string::npos) {	// 是 class 或 struct 都要遞迴所有繼承物件
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
			for (int k = 0; k < ConditionSet[j].Pointer.size(); k++) {	// Pointer // n 次迴圈
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
			for (k = 0; k < ConditionSet[j].Pointer.size(); k++) {	// Pointer // n 次迴圈
				PointerStr = ConditionSet[j].Pointer[k];
				if (PointerStr.empty() || !Utils.IsHex(PointerStr) || !Utils.IsHex(PointerStr)) break;
				Pointer = Utils.StringToHex(PointerStr);

				// 是否是最後一個
				if (k == ConditionSet[j].Pointer.size() - 1) { LastOffset = Pointer; break; }

				// 下一個
				if (!MemMgr.MemReader.ReadMem(Address, Address + Pointer))
				{ 
					EventHandler::NotifyEvent(NotificationConfig::NotiyType::Warning, Utils.StringWrapper("[Skip Condition % d]", j), Utils.StringWrapper("[ Pointer %d ] Invalid memory address", k)); 
					break; 
				}
			}
			if (k != ConditionSet[j].Pointer.size() - 1) continue;	// 迴圈有沒有到第 n-1 個
		}

		// 都是用 16 進位比對
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
			if (ConditionValidation(InspectorConf.ConditionSet)) return;		// 驗證失敗就取消查詢

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

			// 檢查是否有 Object Reference 是否存在
			ObjectData TempObjData;
			if (!StorageMgr.GetObjectDict(ResultAddress, TempObjData, true)) continue;
			if (TempObjData.Name == "InvalidName" or TempObjData.Name == "None") continue;

			// 檢查 Condition 是否通過
			if (InspectorConf.ObjectInstanceSearchMode == InspectorConfig::SearchMode::Condition)
				if (!ScanFilter(InspectorConf.ConditionSet, InstanceAddress)) continue; //任何一個條件沒過就直接丟棄結果

			InspectorConf.ObjectInstanceSearchMap[Utils.HexToString(ResultAddress)] = ResultAddress;
		}
		GetObjectInstanceSearchLock.unlock();
	}
}
