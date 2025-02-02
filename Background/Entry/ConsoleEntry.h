#pragma once
#include "../Core/Parser/FNamePoolParser.h"
#include "../../GUI/Config/NotificationConfig.h"
#include "../../GUI/Config/MainConsoleConfig.h"
#include "../../State/EventHandler.h"
#include "../../State/GUIState.h"
#include "../Interface/ObjectInterface.h"
#include "UtilsEntry.h"

namespace ConsoleEntry
{
	void ClearDumperObjectSerchPanel();
	bool GetGUObjectAddress(std::string ProcName, std::string Mode, std::string InputStr, ObjectData& RetObjData, bool TryParseIfNoFind);

	void GetFNameByID();
	void GetGUObjectByAddress();
	void AddDumperObject();
	void ExpendDumperObject();
	void ParseObject();

	inline void Entry()
	{
		if (MainConsoleState.FNameInfoGetEvent == ProcessState::Start) {
			MainConsoleState.FNameInfoGetEvent = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Get FName By ID");
			GetFNameByID();
			MainConsoleState.FNameInfoGetEvent = ProcessState::Completed;
		}

		if (MainConsoleState.GUObjectInfoGetEvent == ProcessState::Start) {
			MainConsoleState.GUObjectInfoGetEvent = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Get GUObject By Address");
			GetGUObjectByAddress();
			MainConsoleState.GUObjectInfoGetEvent = ProcessState::Completed;
		}

		if (MainConsoleState.GUObjectInfoAddEvent == ProcessState::Start) {
			MainConsoleState.GUObjectInfoAddEvent = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Add Dumper Object");
			AddDumperObject();
			MainConsoleState.GUObjectInfoAddEvent = ProcessState::Completed;
		}

		if (MainConsoleState.GUObjectInfoExpandEvent.State == ProcessState::Start or MainConsoleState.GUObjectInfoExpandEvent.State == ProcessState::Retry) {
			MainConsoleState.GUObjectInfoExpandEvent.State = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Expend Dumper Object");
			ExpendDumperObject();
			MainConsoleState.GUObjectInfoExpandEvent.State = ProcessState::Completed;
		}

		if (MainConsoleState.GUObjectInfoParseEvent == ProcessState::Start) {
			MainConsoleState.GUObjectInfoParseEvent = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Parse Object");
			ParseObject();
			MainConsoleState.GUObjectInfoParseEvent = ProcessState::Completed;
		}
	}
}

void ConsoleEntry::ClearDumperObjectSerchPanel()
{
	MainConsoleConf.GUObjectInfo.ID = "[Result][ ID  ]";
	MainConsoleConf.GUObjectInfo.Type = "[Result][ Type  ]";
	MainConsoleConf.GUObjectInfo.Name = "[Result][ Name  ]";
	MainConsoleConf.GUObjectInfo.FullName = "[Result][ FullName  ]";
	MainConsoleConf.GUObjectInfo.Address = "[Result][ Address  ]";
	MainConsoleConf.GUObjectInfo.Offset = "[Result][ Offset  ]";
	MainConsoleConf.GUObjectInfo.ClassPtr = "[Result][ ClassPtr  ]";
	MainConsoleConf.GUObjectInfo.Funct = "[Result][ Funct  ]";
	MainConsoleConf.GUObjectInfo.Outer = "[Result][ Outer  ]";
	MainConsoleConf.GUObjectInfo.SuperPtr = "[Result][ SuperPtr  ]";
	MainConsoleConf.GUObjectInfo.PropSize = "[Result][ Propsize  ]";
	MainConsoleConf.GUObjectInfo.PropPtr_0 = "[Result][ Property 1  ]";
	MainConsoleConf.GUObjectInfo.PropPtr_1 = "[Result][ Property 2  ]";
	MainConsoleConf.GUObjectInfo.MemberSize = "[Result][ MemberSize  ]";
	MainConsoleConf.GUObjectInfo.MemberPtr_0 = "[Result][ Member 0 ]";
	MainConsoleConf.GUObjectInfo.Bitmask = "[Result][ BitMask  ]";
}


bool ConsoleEntry::GetGUObjectAddress(std::string ProcName, std::string Mode, std::string InputStr, ObjectData& RetObjData, bool TryParseIfNoFind = false) {

	if (Mode == "Address") {
		std::string AddressStr = InputStr;
		if (AddressStr.find("0x") != std::string::npos) AddressStr = AddressStr.substr(2);

		if (AddressStr.length() <= 16 and AddressStr.length() != 0 and Utils.IsHex(AddressStr)) {
			DWORD_PTR Address = std::stoull(AddressStr, nullptr, 16);
			if (StorageMgr.GetObjectDict(Address, RetObjData, TryParseIfNoFind))
				return true;
			else {
				EventHandler::NotifyEvent(NotificationConfig::NotiyType::Error, (TryParseIfNoFind ? "[ " + ProcName + " ][ Failed ] " : "[ Not Find The Object ] "), InputStr);
				MainConsoleConf.GUObjectInfo.ID = (TryParseIfNoFind ? "[ " + ProcName + " ][ Failed ] " : "[ Not Find The Object ] ") + InputStr;
			}
		}
		else if (AddressStr.length() > 16) {
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Error, "[" + ProcName + "][ InputStr Too Long ]", InputStr);
			MainConsoleConf.GUObjectInfo.ID = "[ " + ProcName + " ][ InputStr Too Long ] " + InputStr;
		}
		else {
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Error, "[ " + ProcName + " ][ Invaild ]", InputStr);
			MainConsoleConf.GUObjectInfo.ID = "[ " + ProcName + " ][ Invaild ] " + InputStr;
		}
	}
	else if (Mode == "ID") {
		size_t ID = std::stoull(InputStr, nullptr, 16);
		if (StorageMgr.GetObjectDictByID(ID, RetObjData))
			return true;
		else {
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Error, "[ Not Find The Object ]", InputStr);
			MainConsoleConf.GUObjectInfo.ID = "[ Not Find The Object ] " + InputStr;
		}
	}
	else if (Mode == "Name") {
		if (StorageMgr.GetObjectDictByName(InputStr, RetObjData))
			return true;
		else {
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Error, "[ Not Find The Object ]", InputStr);
			MainConsoleConf.GUObjectInfo.ID = "[ Not Find The Object ] " + InputStr;
		}
	}

	return false;
}

void ConsoleEntry::GetFNameByID()
{
	if (!MainConsoleConf.FNameInfo.InputStr.empty()) {
		long long int FNameID = std::stoull(MainConsoleConf.FNameInfo.InputStr, nullptr, 16);
		std::string FName;
		if (FNameParser.GetFNameStringByID<long long int>(FNameID, FName, true)) {
			MainConsoleConf.FNameInfo.FName = FName;
		}
		else {
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Error, "[ Not Find The FName ]", FName);
			MainConsoleConf.FNameInfo.FName = "[ Not Find The FName ] " + FName;
		}
	}
}


void ConsoleEntry::GetGUObjectByAddress()
{
	// 初始化
	ClearDumperObjectSerchPanel();

	// 字串處理
	ObjectData RetObjData;
	if (GetGUObjectAddress("Get", MainConsoleConf.GUObjectInfo.ModeList[MainConsoleConf.GUObjectInfo.ModeListSelectIndex], std::string(MainConsoleConf.GUObjectInfo.InputStr), RetObjData) == false)
		return;

	//更新資料
	MainConsoleConf.GUObjectInfo.ID = Utils.HexToString(RetObjData.ID);
	MainConsoleConf.GUObjectInfo.Type = RetObjData.Type;
	MainConsoleConf.GUObjectInfo.Name = RetObjData.Name;
	MainConsoleConf.GUObjectInfo.FullName = RetObjData.FullName;
	MainConsoleConf.GUObjectInfo.Address = Utils.HexToString(RetObjData.Address);
	MainConsoleConf.GUObjectInfo.PropSize = Utils.HexToString(RetObjData.PropSize);
	MainConsoleConf.GUObjectInfo.MemberSize = Utils.HexToString(RetObjData.MemberSize);

	if (RetObjData.Offset != 0x0) MainConsoleConf.GUObjectInfo.Offset = Utils.HexToString(RetObjData.Offset);
	if (RetObjData.Outer != 0x0) MainConsoleConf.GUObjectInfo.Outer = Utils.HexToString(RetObjData.Outer);
	if (RetObjData.BitMask != 0x0) MainConsoleConf.GUObjectInfo.Bitmask = Utils.HexToString(RetObjData.BitMask);
	if (RetObjData.Funct != 0x0) MainConsoleConf.GUObjectInfo.Funct = Utils.HexToString(RetObjData.Funct);
	if (RetObjData.MemberPtr.Address != NULL) MainConsoleConf.GUObjectInfo.MemberPtr_0 = Utils.HexToString(RetObjData.MemberPtr.Address);
	if (RetObjData.Property.size() >= 1 and RetObjData.Property[0].Address != NULL) MainConsoleConf.GUObjectInfo.PropPtr_0 = Utils.HexToString(RetObjData.Property[0].Address);
	if (RetObjData.Property.size() >= 2 and RetObjData.Property[1].Address != NULL) MainConsoleConf.GUObjectInfo.PropPtr_1 = Utils.HexToString(RetObjData.Property[1].Address);
	if (RetObjData.ClassPtr.Address != NULL)	MainConsoleConf.GUObjectInfo.ClassPtr = Utils.HexToString(RetObjData.ClassPtr.Address);
	if (RetObjData.SuperPtr.Address != NULL)	MainConsoleConf.GUObjectInfo.SuperPtr = Utils.HexToString(RetObjData.SuperPtr.Address);

}


void ConsoleEntry::AddDumperObject()
{
	ObjectData RetObjData;
	if (GetGUObjectAddress("Add", MainConsoleConf.GUObjectInfo.ModeList[MainConsoleConf.GUObjectInfo.ModeListSelectIndex], std::string(MainConsoleConf.GUObjectInfo.InputStr), RetObjData) == false)
		return;


	//最多同時支援 10 個元素
	if (MainConsoleConf.DumperTable.size() < 10) {
		//檢查是否已經存在於 List	
		bool AlreadyExistCheck = false;
		for (int i = 0; i < MainConsoleConf.DumperTable.size(); i++) {
			if (MainConsoleConf.DumperTable[i].Address == RetObjData.Address) { AlreadyExistCheck = true; break; }
		}
		//如果不存在才加入
		if (!AlreadyExistCheck) {

			//包裝 DumperItem 、 DumperObjectTreeNodes
			// Root Node
			DumperItem DumperItem;
			DumperItem.IsExist = true;
			DumperItem.Index = (int)MainConsoleConf.DumperTable.size();
			DumperItem.Name = RetObjData.Name;
			DumperItem.Type = RetObjData.Type;
			DumperItem.Address = RetObjData.Address;
			DumperItem.Value = RetObjData.Address;
			DumperItem.ObjectAddress = RetObjData.Address;

			//加入 List
			MainConsoleConf.DumperTable.push_back(DumperItem);

		}
		else {
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Warning, "[ Info ]", "Item Already Exist");
		}
	}
	else {
		EventHandler::NotifyEvent(NotificationConfig::NotiyType::Warning, "[ Info ]", "The number of Items has reached the upper limit (Max is 10)");
	}
}


void ConsoleEntry::ExpendDumperObject()
{
	// 檢查是否已存在
	if (MainConsoleConf.DumperTable[MainConsoleState.GUObjectInfoExpandEvent.Index].ObjectMap.find(MainConsoleState.GUObjectInfoExpandEvent.Address) != MainConsoleConf.DumperTable[MainConsoleState.GUObjectInfoExpandEvent.Index].ObjectMap.end()) {
		return;
	}

	ObjectData RetObjData;
	ObjectData TempObjData;
	DWORD_PTR Address_Level_1 = NULL, Address_Level_2 = NULL;
	if (StorageMgr.GetObjectDict(MainConsoleState.GUObjectInfoExpandEvent.Address, RetObjData, true))
	{
		DumperObject TempObjectItem;
		UtilsEntry::ObjectMemberListProc(ProcessClass::C_DumperConsole, RetObjData, TempObjectItem);

		//儲存結果 
		MainConsoleConf.DumperTable[MainConsoleState.GUObjectInfoExpandEvent.Index].ObjectMap[MainConsoleState.GUObjectInfoExpandEvent.Address] = TempObjectItem;
	}
}


void ConsoleEntry::ParseObject()
{
	// Parse 只接受 Address，其他直接判斷 Invaild
	ObjectData RetObjData;
	if (GetGUObjectAddress("Parse", "Address", std::string(MainConsoleConf.GUObjectInfo.InputStr), RetObjData, true) == false)
		return;

	//成功就直接回傳物件資料
	MainConsoleState.GUObjectInfoGetEvent = ProcessState::Start;
}